#include "task_scan_position.h"
#include "wait.h"
#include "video4.h"
#include "serial.h"
#include "scene.h"
#include "data_bus.h"
#include "openCv.h"

#include <QScopeGuard>
#include <QDateTime>

TaskScanPosition::TaskScanPosition(QObject* parent)
    : QObject(parent)
    , _impl(new TaskScanPositionPrivate)
    , _thread(new QThread)
{
    connect(_impl, &TaskScanPositionPrivate::message, this, &TaskScanPosition::message, Qt::QueuedConnection);
    connect(_impl, &TaskScanPositionPrivate::finished, this, &TaskScanPosition::finished, Qt::QueuedConnection);

    connect(_thread.data(), &QThread::finished, _impl, &QObject::deleteLater);

    _impl->moveToThread(_thread.data());
    _thread->start();
}

TaskScanPosition::~TaskScanPosition()
{
    _thread->quit();
    _thread->wait(1000);
}

void TaskScanPosition::run(QPointF pos, QString sceneFile)
{
    _impl->_stop = false;
    QMetaObject::invokeMethod(_impl, "run", Qt::QueuedConnection, Q_ARG(QPointF, pos), Q_ARG(QString, sceneFile));
}

void TaskScanPosition::stopProgram()
{
    _impl->_stop = true;
}


TaskScanPositionPrivate::TaskScanPositionPrivate()
{

}

// Нужно, чтобы было установлено разрешение камере. Если нет, будут проблемы.

void TaskScanPositionPrivate::run(QPointF pos, QString sceneFile)
{
    const auto fin = qScopeGuard([this]{ emit finished(); });

    if (!_someTaskInProgress.tryLock()) return;
    auto mutexUnlock = qScopeGuard([this]{ _someTaskInProgress.unlock(); });

    QTimer statusTimer;
    connect(&statusTimer, &QTimer::timeout, this, []() { serial().write("?\n"); });
    statusTimer.start(100);

    const auto start = QDateTime::currentMSecsSinceEpoch();

    // Не разрешать выезжать за пределы нуля. Пока так, на скорую руку.
    if (pos.x() < 5 || pos.y() < 5)
        return;

    video().start();

    auto connection = connect(&video(), &Video4::captured, this, [sceneFile](QImage img)
    {
        scene().setImage(img.copy());
        db().insert("image_raw_captured", img.copy());
        opencv().appendToBlobDetectorQueue(img.copy(), openIniFile(sceneFile));
    });
    auto guard = qScopeGuard([=]() { disconnect(connection); });

    moveToAndWaitPosition(pos);
    wait(300);
    video().capture();
    waitForSignal(&video(), &Video4::captured, 2000);
    wait(300); // не успевает блоб отдетектироваться
    scene().removeDuplicatedBlobs();

    auto finish = QDateTime::currentMSecsSinceEpoch();

    emit message("program time " + QString::number((finish - start)/1000) + " sec");
    //wait(10);
}
