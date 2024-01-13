#include "task_scan_position.h"
#include "wait.h"
#include "video4.h"
#include "serial.h"
#include "scene.h"

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

void TaskScanPosition::run(QPointF pos)
{
    _impl->_stop = false;
    QMetaObject::invokeMethod(_impl, "run", Qt::QueuedConnection, Q_ARG(QPointF, pos));
}

void TaskScanPosition::stopProgram()
{
    _impl->_stop = true;
}


TaskScanPositionPrivate::TaskScanPositionPrivate()
{

}

void TaskScanPositionPrivate::run(QPointF pos)
{
    const auto fin = qScopeGuard([this]{ emit finished(); });

    if (!_mutex.tryLock()) return;
    auto mutexUnlock = qScopeGuard([this]{ _mutex.unlock(); });

    QTimer statusTimer;
    connect(&statusTimer, &QTimer::timeout, this, []() { serial().write("?\n"); });
    statusTimer.start(100);

    const auto start = QDateTime::currentMSecsSinceEpoch();

    // Не разрешать выезжать за пределы нуля. Пока так, на скорую руку.
    if (pos.x() < 5 || pos.y() < 5)
        return;

    video().start();

    auto connection = connect(&video(), &Video4::captured, &scene(), &Scene::setImage);
    auto guard = qScopeGuard([=]() { disconnect(connection); });

    moveToAndWaitPosition(pos.x(), pos.y());
    wait(300);
    video().capture();
    waitForSignal(&video(), &Video4::captured, 2000);
    wait(300); // не успевает блоб отдетектироваться
    scene().removeDuplicatedBlobs();

    auto finish = QDateTime::currentMSecsSinceEpoch();

    emit message("program time " + QString::number((finish - start)/1000) + " sec");
    //wait(10);
}
