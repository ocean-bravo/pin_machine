#include "test_program.h"
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
#include "search_blobs.h"
#include "update_blobs.h"

namespace {

}

TestProgram::TestProgram(SearchBlobs *sb, UpdateBlobs *ub, QObject* parent)
    : QObject(parent)
    , _impl(new TestProgramPrivate(sb, ub))
    , _thread(new QThread)
{
    connect(_impl, &TestProgramPrivate::message, this, &TestProgram::message, Qt::QueuedConnection);
    connect(_impl, &TestProgramPrivate::finished, this, &TestProgram::finished, Qt::QueuedConnection);

    connect(_thread.data(), &QThread::finished, _impl, &QObject::deleteLater);

    _impl->moveToThread(_thread.data());
    _thread->start();
}

TestProgram::~TestProgram()
{
    _thread->quit();
    _thread->wait(1000);
}

void TestProgram::run(QString program)
{
    _impl->_stop = false;
    QMetaObject::invokeMethod(_impl, "run", Qt::QueuedConnection, Q_ARG(QString, program));
}

void TestProgram::stopProgram()
{
    _impl->_stop = true;
    _impl->_sb->stopProgram();
    _impl->_ub->stopProgram();
}


TestProgramPrivate::TestProgramPrivate(SearchBlobs *sb, UpdateBlobs *ub)
{
    _sb = sb;
    _ub = ub;
}

void TestProgramPrivate::pauseProgram()
{

}

void TestProgramPrivate::wait(int timeout) const
{
    if (timeout <= 0)
        return;

    waitForSignal(this, &TestProgramPrivate::interrupt, timeout);
}

void TestProgramPrivate::run(QString program)
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

        waitForSignal(_sb, &SearchBlobs::finished, 3600*1000);

        if (_stop)
        {
            emit message("program interrupted");
            break;
        }

        _ub->run();

        waitForSignal(_ub, &UpdateBlobs::finished, 3600*1000);

    }

    emit finished();
}


