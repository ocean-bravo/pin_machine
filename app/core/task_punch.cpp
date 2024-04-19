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
    connect(_impl, &TaskPunchPrivate::message,          this, &TaskPunch::message,          Qt::QueuedConnection);
    connect(_impl, &TaskPunchPrivate::finished,         this, &TaskPunch::finished,         Qt::QueuedConnection);
    connect(_impl, &TaskPunchPrivate::isRunningChanged, this, &TaskPunch::isRunningChanged, Qt::QueuedConnection);
    connect(_impl, &TaskPunchPrivate::isPausedChanged,  this, &TaskPunch::isPausedChanged,  Qt::QueuedConnection);

    connect(_thread.data(), &QThread::finished, _impl, &QObject::deleteLater);

    _impl->moveToThread(_thread.data());
    _thread->start();
}

TaskPunch::~TaskPunch()
{
    _thread->quit();
    _thread->wait(1000);
}

void TaskPunch::run(QString punchProgram, int width, int height, QString fourcc)
{
    _impl->_stop = false;
    QMetaObject::invokeMethod(_impl, "run", Qt::QueuedConnection,
                              Q_ARG(QString, punchProgram),
                              Q_ARG(int, width),
                              Q_ARG(int, height),
                              Q_ARG(QString, fourcc));
}

void TaskPunch::stopProgram()
{
    _impl->_stop = true;
}

bool TaskPunch::isRunning() const
{
    return _impl->_running;
}

bool TaskPunch::stepByStep() const
{
    return _impl->_stepByStep;
}

void TaskPunch::setStepByStep(bool state)
{
    if (_impl->_stepByStep == state)
        return;

    _impl->_stepByStep = state;
    emit stepByStepChanged();
}

bool TaskPunch::noPunch() const
{
    return _impl->_noPunch;
}

void TaskPunch::setNoPunch(bool state)
{
    if (_impl->_noPunch == state)
        return;

    _impl->_noPunch = state;
    emit noPunchChanged();
}

bool TaskPunch::isPaused() const
{
    return _impl->_isPaused;
}

void TaskPunch::setIsPaused(bool state)
{
    if (_impl->_isPaused == state)
        return;

    _impl->_isPaused = state;
    emit isPausedChanged();
}



TaskPunchPrivate::TaskPunchPrivate()
{

}

void TaskPunchPrivate::waitForNextStep() // stopEx
{
    if (_stepByStep)
    {
        _isPaused = true;
        emit isPausedChanged();
    }

    if (_isPaused)
    {
        qd() << "waiting for continue...";

        QTimer timer;
        QEventLoop loop;
        QMetaObject::Connection conn = QObject::connect(&timer, &QTimer::timeout, &loop, [&loop, this]()
        {
            if (!_isPaused || _stop)
                loop.quit();
        });

        auto guard = qScopeGuard([=]() { QObject::disconnect(conn); });

        timer.start(50);
        loop.exec();

        if (_stop)
            throw stopEx();
    }
}

void TaskPunchPrivate::run(QString punchProgram, int width, int height, QString fourcc)
{
    const auto fin = qScopeGuard([this]{ emit finished(); });

    if (!_someTaskInProgress.tryLock()) return;
    auto someTaskInProgress = qScopeGuard([this]{ _someTaskInProgress.unlock(); });

    _running = true;
    emit isRunningChanged();
    auto running = qScopeGuard([this]{ _running = false; emit isRunningChanged(); });

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

        db().insert("resolution_width", width);
        db().insert("resolution_height", height);

        video().changeCamera(cameraId(), width, height, fourcc);
        video().start();

        auto connection = connect(&video(), &Video4::capturedSmallRegion, this, [](QImage img) { scene().setImage(img.copy()); });
        auto guard = qScopeGuard([=]() { disconnect(connection); });



        // Эта точка должна быть в пути! Убрать это отсюда потом
        const QPointF startPoint = db().value("punchpath_start_point").toPointF();
        // qd() << "На стартовой позиции";
        moveToAndWaitPosition(startPoint); // Приехали на позицию

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
            waitForNextStep();

            BlobItem* realFiducialBlob = scene().addBlobCopy(referenceFiducialBlob, true); // Родитель - сцена

            runOnThreadWait(&scene(), [=]() { realFiducialBlob->setRealFiducial(true); });

            //const QVariantMap options = openIniFile(realFiducialBlob->sceneFileName());
            //qd() << "real fiducial blob scene filename: " << realFiducialBlob->sceneFileName();

            updateBlobPosition5x(realFiducialBlob);
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

        // Теперь определяем реальные координаты точек для забивания, добавляем сдвиг на инструмент и поехали забивать.
        int count  = 0;

        QList<BlobItem*> blobs = db().value("punchpath").value<QList<BlobItem*>>();

        if (blobs.isEmpty())
            blobs = scene().punchBlobs();

        const QStringList punchCode = punchProgram.split("\n", Qt::SkipEmptyParts);
        const double dx = db().value("punch_tool_shift_dx").toDouble(); // сдвиг инструмента
        const double dy = db().value("punch_tool_shift_dy").toDouble(); // сдвиг инструмента

        for (BlobItem* blob : qAsConst(blobs))
        {
            if (db().value("check_every_blob").toBool())
                updateBlobPosition5x(blob);

            waitForNextStep(); // Перед ехать

            moveToAndWaitPosition(blob->scenePos() - QPointF(dx, dy)); // Приехали на позицию

            if (!_noPunch)
            {
                waitForNextStep(); // Перед панчем

                for (const QString& gCode : punchCode)
                {
                    serial().write(gCode.toLatin1() + "\n");
                    emit message(gCode);
                    const double z = extractFromGcodeZ(gCode);

                    waitPosZ(z, _stop);
                }
            }
            ++count;
        }

        waitForNextStep(); // Перед ехать

        moveToAndWaitPosition(db().value("punchpath_start_point").toPointF()); // Поехали назад в домашнюю точку

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
    emit message("punch blobs finished");
    emit message("time " + QString::number(std::floor((finish-start)/1000)) + " sec");
}
