#include "task_punch.h"
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

TaskPunch::TaskPunch(QObject* parent)
    : QObject(parent)
    , _impl(new TaskPunchPrivate)
    , _thread(new QThread)
{
    connect(_impl, &TaskPunchPrivate::message, this, &TaskPunch::message, Qt::QueuedConnection);
    connect(_impl, &TaskPunchPrivate::finished, this, &TaskPunch::finished, Qt::QueuedConnection);

    connect(_thread.data(), &QThread::finished, _impl, &QObject::deleteLater);

    _impl->moveToThread(_thread.data());
    _thread->start();
}

TaskPunch::~TaskPunch()
{
    _thread->quit();
    _thread->wait(1000);
}

void TaskPunch::run()
{
    _impl->_stop = false;
    QMetaObject::invokeMethod(_impl, "run", Qt::QueuedConnection);
}

void TaskPunch::stopProgram()
{
    _impl->_stop = true;
}


TaskPunchPrivate::TaskPunchPrivate()
{

}

void TaskPunchPrivate::run()
{
    // Разница в диаметрах блобов, больше которой считается что блоб неправильный
    static const double wrongBlobDiaError = 0.3;

    if (!_mutex.tryLock()) return;
    auto mutexUnlock = qScopeGuard([this]{ _mutex.unlock(); });

    video().stop();

    db().insert("resolution_width", 1280);
    db().insert("resolution_height", 960);
    db().insert("pixel_size", 0.0107);

    video().changeCamera(0, 1280, 960, "YUYV"); // НУжен номер девайса
    video().start();

    QTimer statusTimer;
    connect(&statusTimer, &QTimer::timeout, this, []() { serial().write("?\n"); });
    statusTimer.start(100);

    const auto start = QDateTime::currentMSecsSinceEpoch();

    auto connection = connect(&video(), &Video4::capturedSmallRegion, &scene(), &Scene::setImage);
    auto guard = qScopeGuard([=]() { disconnect(connection); });

    // Удаляю все реальные опорные точки, оставшиеся на сцене с предыдущего раза
    every<BlobItem>(scene().items(), [](BlobItem* blob) { if (blob->isRealFiducial()) blob->deleteLater(); });

    qd() << "board pos " << scene().board()->pos() << " angle " << scene().board()->rotation();

    // Восстанавливаю поворот и позицию платы с предудущего раза.
    // Надо выполнять в потоке сцены, там внутри запускается какой то таймер
    runOnThreadWait(&scene(), []() { scene().board()->setTransformOriginPoint({0,0});});
    runOnThreadWait(&scene(), []() { scene().board()->setRotation(0);});
    runOnThreadWait(&scene(), []() { scene().board()->setPos({0,0});});
    //wait(50);

    qd() << "board pos " << scene().board()->pos() << " angle " << scene().board()->rotation();

    QList<BlobItem*> referenceFiducialBlobs;

    every<BlobItem>(scene().items(), [&referenceFiducialBlobs](BlobItem* blob)
    {
        if (blob->isFiducial())
            referenceFiducialBlobs.append(blob);
    });

    int count  = 0;
    QList<std::tuple<BlobItem*, BlobItem*>> fiducialBlobs; // Пары опорных точек - идеальная и реальная
    for (BlobItem* referenceFiducialBlob  : qAsConst(referenceFiducialBlobs))
    {
        if (_stop)
        {
            emit message("program interrupted");
            break;
        }

        ++count;

        BlobItem* realFiducialBlob = scene().addBlobCopy(referenceFiducialBlob, true); // Родитель - сцена
        //realFiducialBlob->setRealFiducial(true);
        runOnThreadWait(&scene(), [=]() { realFiducialBlob->setRealFiducial(true); });

        updateBlobPosition(realFiducialBlob);
        int result = updateBlobPosition(realFiducialBlob);
        if (result > 0)
        {
            result = updateBlobPosition(realFiducialBlob);
            if (result > 0)
                result = updateBlobPosition(realFiducialBlob);
        }
        fiducialBlobs.append(std::make_tuple(referenceFiducialBlob, realFiducialBlob));
    }

//    { BlobItem* bl1 = scene().addBlob(100,0,2);
//      bl1->setFiducial(true);
//      BlobItem* bl2 = scene().addBlob(5,6,2, true);
//      bl2->setFiducial(true);
//      bl2->setRotation(45);
//      fiducialBlobs.append(std::make_tuple(bl1, bl2));
//    }

//    { BlobItem* bl1 = scene().addBlob(50,150,2);
//      bl1->setFiducial(true);
//      BlobItem* bl2 = scene().addBlob(110,82,2, true);
//      bl2->setFiducial(true);
//      bl2->setRotation(45);
//      fiducialBlobs.append(std::make_tuple(bl1, bl2));
//    }

    // Теперь надо совместить каждую пару referenceFiducialBlob и realFiducialBlob.
    // В идеале, таких пар должно быть 2. Меньше вообще нельзя, а больше смысла не имеет, только сложнее расчет поворота платы.
    // referenceFiducialBlob привязана к контуру платы (имеет его в качестве родителя).
    // realFiducialBlob  не привязана никуда.

    if (fiducialBlobs.size() != 2)
        return;

    QPointF firstRef = std::get<0>(fiducialBlobs[0])->pos();
    QPointF firstReal = std::get<1>(fiducialBlobs[0])->pos();

    QPointF secondRef = std::get<0>(fiducialBlobs[1])->pos();
    QPointF secondReal = std::get<1>(fiducialBlobs[1])->pos();

    // 1.
    // Передвигаем плату в 1-ю реальную fid точку. 1-ые реальная и идеальная точки совпали.
    // Реальная точка стоит, к ней двигаем плату с идеальной точкой
    runOnThreadWait(&scene(), [=]() { scene().board()->moveBy(firstReal.x() - firstRef.x(), firstReal.y() - firstRef.y()); });


    // 2.
    // Помещаем transform origin всей платы в первую идеальную fid точку.
    runOnThreadWait(&scene(), [=]() { scene().board()->setTransformOriginPoint(firstRef); });

    double angleReal = QLineF(firstReal, secondReal).angle();
    double angleRef = QLineF(firstRef, secondRef).angle();

    double deltaAngle = angleReal - angleRef;

    // Довернули плату до реального угла
    runOnThreadWait(&scene(), [=]() { scene().board()->setRotation(-deltaAngle); });


    // 3. Перемещаю идеальные опорные точки по линии, соединяющей опорные точки, на половину разницы расстояний
    double distanceReal = QLineF(firstReal, secondReal).length();
    double distanceRef = QLineF(firstRef, secondRef).length();
    double distanceDelta = distanceReal - distanceRef;

    runOnThreadWait(&scene(), [=]() { scene().board()->moveBy(std::sin(angleReal)*distanceDelta/2, std::cos(angleReal)*distanceDelta/2); });

    // Теперь определяем реальные координаты точек для забивания и посещаем их.
    every<BlobItem>(scene().items(), [this](BlobItem* blob)
    {
        if (blob->isPunch())
        {
            moveTo(blob->scenePos().x(), blob->scenePos().y());
            waitForGetPosition(blob->scenePos().x(), blob->scenePos().y());
            wait(500);
        }
    });

    qd() << "board pos " << scene().board()->pos() << " angle " << scene().board()->rotation();

    const auto finish = QDateTime::currentMSecsSinceEpoch();
    emit message("punch blobs finished");
    emit message("time " + QString::number(std::floor((finish-start)/1000)) + " sec");
    emit message("count " + QString::number(count));

    emit finished();
}
