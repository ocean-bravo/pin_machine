#include "task_update.h"
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
#include <QMetaMethod>
#include <QScopeGuard>
#include <QDateTime>


#include "common.h"

TaskUpdate::TaskUpdate(QObject* parent)
    : QObject(parent)
    , _impl(new TaskUpdatePrivate)
    , _thread(new QThread)
{
    connect(_impl, &TaskUpdatePrivate::message, this, &TaskUpdate::message, Qt::QueuedConnection);
    connect(_impl, &TaskUpdatePrivate::finished, this, &TaskUpdate::finished, Qt::QueuedConnection);

    connect(_thread.data(), &QThread::finished, _impl, &QObject::deleteLater);

    _impl->moveToThread(_thread.data());
    _thread->start();
}

TaskUpdate::~TaskUpdate()
{
    _thread->quit();
    _thread->wait(1000);
}

void TaskUpdate::run()
{
    _impl->_stop = false;
    QMetaObject::invokeMethod(_impl, "run", Qt::QueuedConnection);
}

void TaskUpdate::stopProgram()
{
    _impl->_stop = true;
}


TaskUpdatePrivate::TaskUpdatePrivate()
{

}

void TaskUpdatePrivate::run()
{
    if (!_mutex.tryLock()) return;
    auto mutexUnlock = qScopeGuard([this]{ _mutex.unlock(); });

    video().stop();

    db().insert("resolution_width", 1280);
    db().insert("resolution_height", 960);
    db().insert("pixel_size", 0.0107);

    video().changeCamera(0, 1280, 960, "YUYV"); // НУжен номер девайса
    video().start();

    QTimer statusTimer;
    connect(&statusTimer, &QTimer::timeout, this, []() { serial().write("?\n"); });
    statusTimer.start(100);


    const auto start = QDateTime::currentMSecsSinceEpoch();

    scene().removeDuplicatedBlobs();

    auto connection = connect(&video(), &Video4::capturedSmallRegion, &scene(), &Scene::setImage);
    auto guard = qScopeGuard([=]() { disconnect(connection); });


    QList<QGraphicsItem*> itemsToUpdate;

    if (!scene().selectedItems().isEmpty())
        itemsToUpdate = scene().selectedItems();
    else
        itemsToUpdate = scene().items();

    int count  = 0;
    for (QGraphicsItem* item  : qAsConst(itemsToUpdate))
    {
        if (_stop)
        {
            emit message("program interrupted");
            break;
        }

        if (isNot<BlobItem>(item))
            continue;

        ++count;
        BlobItem* blob = dynamic_cast<BlobItem*>(item);

        updateBlobPosition(blob);
        int result = updateBlobPosition(blob);
        if (result > 0)
        {
            result = updateBlobPosition(blob);
            if (result > 0)
                result = updateBlobPosition(blob);
        }
    }

    const auto finish = QDateTime::currentMSecsSinceEpoch();
    emit message("update blobs finished");
    emit message("time " + QString::number(std::floor((finish-start)/1000)) + " sec");
    emit message("count " + QString::number(count));

    emit finished();
}
