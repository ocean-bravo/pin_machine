#include "task_test.h"
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


TaskTest::TaskTest(TaskScan *sb, TaskUpdate *ub, QObject* parent)
    : QObject(parent)
    , _impl(new TaskTestPrivate(sb, ub))
    , _thread(new QThread)
{
    connect(_impl, &TaskTestPrivate::message, this, &TaskTest::message, Qt::QueuedConnection);
    connect(_impl, &TaskTestPrivate::finished, this, &TaskTest::finished, Qt::QueuedConnection);

    connect(_thread.data(), &QThread::finished, _impl, &QObject::deleteLater);

    _impl->moveToThread(_thread.data());
    _thread->start();
}

TaskTest::~TaskTest()
{
    _thread->quit();
    _thread->wait(1000);
}

void TaskTest::run(QString program)
{
    _impl->_stop = false;
    QMetaObject::invokeMethod(_impl, "run", Qt::QueuedConnection, Q_ARG(QString, program));
}

void TaskTest::stopProgram()
{
    _impl->_stop = true;
    _impl->_sb->stopProgram();
    _impl->_ub->stopProgram();
}


TaskTestPrivate::TaskTestPrivate(TaskScan *sb, TaskUpdate *ub)
{
    _sb = sb;
    _ub = ub;
}

void TaskTestPrivate::run(QString program)
{
    if (!_mutex.tryLock()) return;
    auto mutexUnlock = qScopeGuard([this]{ _mutex.unlock(); });

    while(true)
    {
        if (_stop)
        {
            emit message("program interrupted");
            break;
        }

        _sb->run(program);

        waitForSignal(_sb, &TaskScan::finished, 3600*1000);

        if (_stop)
        {
            emit message("program interrupted");
            break;
        }

        _ub->run();

        waitForSignal(_ub, &TaskUpdate::finished, 3600*1000);

    }

    emit finished();
}
