#include "task_find_blob.h"
#include "wait.h"
#include "video4.h"
#include "serial.h"
#include "scene.h"
#include "data_bus.h"
#include "openCv.h"

#include <QScopeGuard>
#include <QDateTime>
#include <QVariantMap>

#include <QGraphicsPixmapItem>

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

void TaskFindBlob::run(QVariantMap options, bool slow)
{
    _impl->_stop = false;
    QMetaObject::invokeMethod(_impl, "run", Qt::QueuedConnection, Q_ARG(QVariantMap, options), Q_ARG(bool, slow));
}

void TaskFindBlob::stopProgram()
{
    _impl->_stop = true;
}


TaskFindBlobPrivate::TaskFindBlobPrivate()
{

}

void TaskFindBlobPrivate::run(QVariantMap options, bool slow)
{
    const auto fin = qScopeGuard([this]{ emit finished(); });

    if (!_mutex.tryLock()) return;
    auto mutexUnlock = qScopeGuard([this]{ _mutex.unlock(); });

    const auto start = QDateTime::currentMSecsSinceEpoch();

    QList<QGraphicsPixmapItem*> pixs = scene().pixmaps();

    // подать на обнаружение
    qd() << "images to find " << pixs.size();

    for (QGraphicsPixmapItem* pixmap : pixs)
    {
        QImage img = pixmap->pixmap().toImage().convertToFormat(QImage::Format_RGB888, Qt::ColorOnly);

        db().insert("image_raw_captured", img.copy());
        opencv().appendToBlobDetectorQueue(img.copy(), options);
        wait(500); //(slow ? 2000 : 10);

        if (_stop)
        {
            emit message("program interrupted");
            break;
        }
    }

    //scene().removeDuplicatedBlobs();

    auto finish = QDateTime::currentMSecsSinceEpoch();

    emit message("program time " + QString::number((finish - start)/1000) + " sec");
}
