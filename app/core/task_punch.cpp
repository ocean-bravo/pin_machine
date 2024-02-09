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
#include "settings.h"

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

void TaskPunch::run(QString punchProgram)
{
    _impl->_stop = false;
    QMetaObject::invokeMethod(_impl, "run", Qt::QueuedConnection, Q_ARG(QString, punchProgram));
}

void TaskPunch::stopProgram()
{
    _impl->_stop = true;
}


TaskPunchPrivate::TaskPunchPrivate()
{

}

void TaskPunchPrivate::run(QString punchProgram)
{
    const auto fin = qScopeGuard([this]{ emit finished(); });

    if (!_mutex.tryLock()) return;
    auto mutexUnlock = qScopeGuard([this]{ _mutex.unlock(); });

    QTimer statusTimer;
    connect(&statusTimer, &QTimer::timeout, this, []() { serial().write("?\n"); });
    statusTimer.start(100);

    const auto start = QDateTime::currentMSecsSinceEpoch();

    // Мешается GUI. При обнуражении камеры идет ее запуск. Решить бы это как то.
    video().reloadDevices();
    wait(500);
    video().stop();

    db().insert("resolution_width", 1280);
    db().insert("resolution_height", 960);

    video().changeCamera(cameraId(), 1280, 960, "YUYV");
    video().start();

    auto connection = connect(&video(), &Video4::capturedSmallRegion, &scene(), &Scene::setImage);
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

        updateBlobPosition(realFiducialBlob);
        updateBlobPosition(realFiducialBlob);
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

    // Теперь надо совместить каждую пару referenceFiducialBlob и realFiducialBlob.
    // В идеале, таких пар должно быть 2. Меньше вообще нельзя, а больше смысла не имеет, только сложнее расчет поворота платы.
    // referenceFiducialBlob привязана к контуру платы (имеет его в качестве родителя).
    // realFiducialBlob  не привязана никуда.

    if (fiducialBlobs.size() != 2)
        return;

    QPointF firstRef = std::get<0>(fiducialBlobs[0])->scenePos();
    QPointF firstReal = std::get<1>(fiducialBlobs[0])->scenePos();

    BlobItem* secondRef = std::get<0>(fiducialBlobs[1]);
    BlobItem* secondReal = std::get<1>(fiducialBlobs[1]);

    algorithmMatchPoints(firstRef, firstReal, secondRef, secondReal);

    // Теперь определяем реальные координаты точек для забивания, добавляем сдвиг на инструмент и поехали забивать.
    int count  = 0;
    const double dx = db().value("punch_tool_shift_dx_mm").toDouble(); // сдвиг инструмента
    const double dy = db().value("punch_tool_shift_dy_mm").toDouble(); // сдвиг инструмента
    const QStringList punchCode = punchProgram.split("\n", Qt::SkipEmptyParts);

    QList<BlobItem*> blobs = db().value("punchpath").value<QList<BlobItem*>>();

    if (blobs.isEmpty())
        blobs = scene().punchBlobs();

    for (BlobItem* blob : qAsConst(blobs))
    {
        moveToAndWaitPosition(blob->scenePos() + QPointF(dx, dy)); // Приехали на позицию
        if (_stop) { emit message("program interrupted"); return; }
        for (const QString& gCode : punchCode)
        {
            if (_stop) { emit message("program interrupted"); return; }
            serial().write(gCode.toLatin1() + "\n");
            emit message(gCode);
            const double z = extractFromGcodeZ(gCode);
            waitPosZ(z);
        }
        ++count;
    }

    // Поехали назад в домашнюю точку
    moveToAndWaitPosition(db().value("punchpath_start_point").toPointF());

    qd() << "board pos " << scene().board()->pos() << " angle " << scene().board()->rotation();

    const auto finish = QDateTime::currentMSecsSinceEpoch();
    emit message("punch blobs finished");
    emit message("time " + QString::number(std::floor((finish-start)/1000)) + " sec");
    emit message("count " + QString::number(count));
}
