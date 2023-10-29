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

namespace {

}

TestProgram::TestProgram(QObject* parent)
    : QObject(parent)
    , _impl(new TestProgramPrivate)
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

void TestProgram::run()
{
    QMetaObject::invokeMethod(_impl, "run", Qt::QueuedConnection);
}

void TestProgram::stopProgram()
{
    _impl->stopProgram = true;
}


TestProgramPrivate::TestProgramPrivate()
{

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

void TestProgramPrivate::run()
{


    emit finished();
}


