#include "task_find_blob.h"
#include "wait.h"
#include "video4.h"
#include "serial.h"
#include "scene.h"
#include "data_bus.h"
#include "openCv.h"

#include <QScopeGuard>
#include <QDateTime>

TaskFindBlob::TaskFindBlob(QObject* parent)
    : QObject(parent)
    , _impl(new TaskFindBlobPrivate)
    , _thread(new QThread)
{
    connect(_impl, &TaskFindBlobPrivate::message, this, &TaskFindBlob::message, Qt::QueuedConnection);
    connect(_impl, &TaskFindBlobPrivate::finished, this, &TaskFindBlob::finished, Qt::QueuedConnection);

    connect(_thread.data(), &QThread::finished, _impl, &QObject::deleteLater);

    _impl->moveToThread(_thread.data());
    _thread->start();
}

TaskFindBlob::~TaskFindBlob()
{
    _thread->quit();
    _thread->wait(1000);
}

void TaskFindBlob::run(QPointF pos)
{
    _impl->_stop = false;
    QMetaObject::invokeMethod(_impl, "run", Qt::QueuedConnection, Q_ARG(QPointF, pos));
}

void TaskFindBlob::stopProgram()
{
    _impl->_stop = true;
}


TaskFindBlobPrivate::TaskFindBlobPrivate()
{

}

// Нужно, чтобы было установлено разрешение камере. Если нет, будут проблемы.

void TaskFindBlobPrivate::run(QPointF pos)
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

    auto connection = connect(&video(), &Video4::captured, this, [](QImage img)
    {
        scene().setImage(img); // копия не нужна. Внутри делается копия
        db().insert("image_raw_captured", img.copy());
        opencv().blobDetectorCaptured(img.copy());
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
