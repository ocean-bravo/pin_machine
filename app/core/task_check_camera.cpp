#include "task_check_camera.h"
#include "wait.h"
#include "video4.h"
#include "serial.h"
#include "utils.h"
#include "utils2.h"
#include "openCv.h"
#include "data_bus.h"
#include "scene.h"
#include "blob_item.h"

#include <QEventLoop>
#include <QTimer>
#include <QMetaMethod>
#include <QScopeGuard>
#include <QDateTime>

#include "common.h"

TaskCheckCamera::TaskCheckCamera(QObject* parent)
    : QObject(parent)
    , _impl(new TaskCheckCameraPrivate)
    , _thread(new QThread)
{
    connect(_impl, &TaskCheckCameraPrivate::message, this, &TaskCheckCamera::message, Qt::QueuedConnection);
    connect(_impl, &TaskCheckCameraPrivate::finished, this, &TaskCheckCamera::finished, Qt::QueuedConnection);

    connect(_thread.data(), &QThread::finished, _impl, &QObject::deleteLater);

    _impl->moveToThread(_thread.data());
    _thread->start();
}

TaskCheckCamera::~TaskCheckCamera()
{
    _thread->quit();
    _thread->wait(1000);
}

void TaskCheckCamera::run(QVariantMap options)
{
    _impl->_stop = false;
    QMetaObject::invokeMethod(_impl, "run", Qt::QueuedConnection, Q_ARG(QVariantMap, options));
}

void TaskCheckCamera::stopProgram()
{
    _impl->_stop = true;
}


TaskCheckCameraPrivate::TaskCheckCameraPrivate()
{

}

void TaskCheckCameraPrivate::run(QVariantMap options)
{
    // Разница в диаметрах блобов, больше которой считается что блоб неправильный
    //static const double wrongBlobDiaError = 0.3;

    const auto fin = qScopeGuard([this]{ emit finished(); });

    if (!_someTaskInProgress.tryLock()) return;
    auto mutexUnlock = qScopeGuard([this]{ _someTaskInProgress.unlock(); });

    QTimer statusTimer;
    connect(&statusTimer, &QTimer::timeout, this, []() { serial().write("?\n"); });
    statusTimer.start(100);

    const auto start = QDateTime::currentMSecsSinceEpoch();

    try
    {
        // Мешается GUI. При обнуражении камеры идет ее запуск. Решить бы это как то.
        video().reloadDevices();
        wait(500);
        video().stop();

        db().insert("resolution_width", 800);
        db().insert("resolution_height", 600);

        video().changeCamera(cameraId(), 800, 600, "YUYV");
        video().start();

        auto connection = connect(&video(), &Video4::capturedSmallRegion, this, [](QImage img) { scene().setImage(img.copy()); });
        auto guard = qScopeGuard([=]() { disconnect(connection); });

        // Удаляю все реальные опорные точки, оставшиеся на сцене с предыдущего раза
        every<BlobItem>(scene().items(), [](BlobItem* blob) { if (blob->isRealFiducial()) blob->deleteLater(); });

        qd() << "board pos " << scene().board()->pos() << " angle " << scene().board()->rotation();

        // Восстанавливаю поворот и позицию платы с предудущего раза.
        // Надо выполнять в потоке сцены, там внутри запускается какой то таймер
        runOnThreadWait(&scene(), []() { scene().board()->setRotation(0);});
        runOnThreadWait(&scene(), []() { scene().board()->setPos({0,0});});
        runOnThreadWait(&scene(), []() { scene().board()->setTransformOriginPoint({0,0});});

        qd() << "board pos " << scene().board()->pos() << " angle " << scene().board()->rotation();

        QList<BlobItem*> referenceFiducialBlobs;

        every<BlobItem>(scene().items(), [&referenceFiducialBlobs](BlobItem* blob)
        {
            if (blob->isFiducial())
                referenceFiducialBlobs.append(blob);
        });


        QList<std::tuple<BlobItem*, BlobItem*>> fiducialBlobs; // Пары опорных точек - идеальная и реальная
        for (BlobItem* referenceFiducialBlob  : qAsConst(referenceFiducialBlobs))
        {
            if (_stop) { emit message("program interrupted"); break; }

            BlobItem* realFiducialBlob = scene().addBlobCopy(referenceFiducialBlob, true); // Родитель - сцена
            //realFiducialBlob->setRealFiducial(true);
            runOnThreadWait(&scene(), [=]() { realFiducialBlob->setRealFiducial(true); });

            updateBlobPosition(realFiducialBlob, options);
            updateBlobPosition(realFiducialBlob, options);
            updateBlobPosition(realFiducialBlob, options);
            int result = updateBlobPosition(realFiducialBlob, options);
            if (result > 0)
            {
                result = updateBlobPosition(realFiducialBlob, options);
                if (result > 0)
                    result = updateBlobPosition(realFiducialBlob, options);
            }
            fiducialBlobs.append(std::make_tuple(referenceFiducialBlob, realFiducialBlob));
        }

        // Теперь надо совместить каждую пару referenceFiducialBlob и realFiducialBlob.
        // В идеале, таких пар должно быть 2. Меньше вообще нельзя, а больше смысла не имеет, только сложнее расчет поворота платы.
        // referenceFiducialBlob привязана к контуру платы (имеет его в качестве родителя).
        // realFiducialBlob  не привязана никуда.

        if (fiducialBlobs.size() != 2)
        {
            db().insert("messagebox", "fiducial точек должно быть 2");
            return;
        }

        QPointF firstRef = std::get<0>(fiducialBlobs[0])->scenePos();
        QPointF firstReal = std::get<1>(fiducialBlobs[0])->scenePos();

        BlobItem* secondRef = std::get<0>(fiducialBlobs[1]);
        BlobItem* secondReal = std::get<1>(fiducialBlobs[1]);

        algorithmMatchPoints(firstRef, firstReal, secondRef, secondReal);


        // сделать тест доворота.
        // разные точки в разных местах.

        //И сделать тест разброса определения координат блоба

        // Теперь определяем реальные координаты точек для забивания и посещаем их.

        QList<BlobItem*> blobs = db().value("punchpath").value<QList<BlobItem*>>();

        if (blobs.isEmpty())
            blobs = scene().punchBlobs();

        int count  = 0;
        for (BlobItem* blob : qAsConst(blobs))
        {
            moveToAndWaitPosition(blob->scenePos());
            wait(500);
            ++count;
        }

        //qd() << "board pos " << scene().board()->pos() << " angle " << scene().board()->rotation();

        emit message("count " + QString::number(count));
    }
    catch (const stopEx& e)
    {
        qd() << "program interrupted";
        emit message("program interrupted");
    }
    catch (...)
    {

    }

    const auto finish = QDateTime::currentMSecsSinceEpoch();
    emit message("check camera finished");
    emit message("time " + QString::number(std::floor((finish-start)/1000)) + " sec");
}
