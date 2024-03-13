#include "task_test_scan_update_cycle.h"
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
#include <QRegExp>
#include <QMetaMethod>
#include <QScopeGuard>
#include <QDateTime>
#include <QGraphicsScene>
#include <QGraphicsEllipseItem>

#include "common.h"
#include "task_scan.h"
#include "task_update.h"


TaskTestScanUpdateCycle::TaskTestScanUpdateCycle(TaskScan *sb, TaskUpdate *ub, QObject* parent)
    : QObject(parent)
    , _impl(new TaskTestScanUpdateCyclePrivate(sb, ub))
    , _thread(new QThread)
{
    connect(_impl, &TaskTestScanUpdateCyclePrivate::message, this, &TaskTestScanUpdateCycle::message, Qt::QueuedConnection);
    connect(_impl, &TaskTestScanUpdateCyclePrivate::finished, this, &TaskTestScanUpdateCycle::finished, Qt::QueuedConnection);

    connect(_thread.data(), &QThread::finished, _impl, &QObject::deleteLater);

    _impl->moveToThread(_thread.data());
    _thread->start();
}

TaskTestScanUpdateCycle::~TaskTestScanUpdateCycle()
{
    _thread->quit();
    _thread->wait(1000);
}

void TaskTestScanUpdateCycle::run(QString program)
{
    _impl->_stop = false;
    QMetaObject::invokeMethod(_impl, "run", Qt::QueuedConnection, Q_ARG(QString, program));
}

void TaskTestScanUpdateCycle::stopProgram()
{
    _impl->_stop = true;
    _impl->_sb->stopProgram();
    _impl->_ub->stopProgram();
}


TaskTestScanUpdateCyclePrivate::TaskTestScanUpdateCyclePrivate(TaskScan *sb, TaskUpdate *ub)
{
    _sb = sb;
    _ub = ub;
}

void TaskTestScanUpdateCyclePrivate::run(QString program, QVariantMap options)
{
    const auto fin = qScopeGuard([this]{ emit finished(); });

    if (!_mutex.tryLock()) return;
    auto mutexUnlock = qScopeGuard([this]{ _mutex.unlock(); });

    while(true)
    {
        if (_stop)
        {
            emit message("program interrupted");
            break;
        }

        _sb->run(program, 800, 600, "YUYV");

        waitForSignal(_sb, &TaskScan::finished, 3600*1000);

        if (_stop)
        {
            emit message("program interrupted");
            break;
        }

        _ub->run(1280, 960, "YUYV");

        waitForSignal(_ub, &TaskUpdate::finished, 3600*1000);

    }
}
