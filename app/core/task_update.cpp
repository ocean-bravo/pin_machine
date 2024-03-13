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

void TaskUpdate::run(int width, int height, QString fourcc)
{
    _impl->_stop = false;
    QMetaObject::invokeMethod(_impl, "run", Qt::QueuedConnection,
                              Q_ARG(int, width),
                              Q_ARG(int, height),
                              Q_ARG(QString, fourcc));
}

void TaskUpdate::stopProgram()
{
    _impl->_stop = true;
}


TaskUpdatePrivate::TaskUpdatePrivate()
{

}

void TaskUpdatePrivate::run(int width, int height, QString fourcc)
{
    const auto fin = qScopeGuard([this]{ emit finished(); });

    if (!_mutex.tryLock()) return;
    auto mutexUnlock = qScopeGuard([this]{ _mutex.unlock(); });

    QTimer statusTimer;
    connect(&statusTimer, &QTimer::timeout, this, []() { serial().write("?\n"); });
    statusTimer.start(100);

    auto start = QDateTime::currentMSecsSinceEpoch();

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

    QList<BlobItem*> blobs;

    every<BlobItem>(scene().items(), [&blobs](BlobItem* blob)
    {
        if (blob->isFiducial() || blob->isPunch())
            blobs.append(blob);
    });

    if (blobs.isEmpty())
    {
        every<BlobItem>(scene().items(), [&blobs](BlobItem* blob)
        {
            blobs.append(blob);
        });
    }

    int count  = 0;

    QPointF startPoint = currPos();
    QList<BlobItem*> orderedBlobsToUpdate = findShortestPath(blobs, startPoint);

    for (BlobItem* blob : orderedBlobsToUpdate)
    {
        if (_stop)
        {
            emit message("program interrupted");
            return;
        }

        ++count;

        const QVariantMap options = openIniFile(blob->sceneFileName());
        updateBlobPosition(blob, options);
        int result = updateBlobPosition(blob, options);
        if (result > 0)
        {
            result = updateBlobPosition(blob, options);
            if (result > 0)
                result = updateBlobPosition(blob, options);
        }

        const double x = blob->scenePos().x();
        const double y = blob->scenePos().y();
        qd() << QString("updated blob position: %1 %2").arg(toReal3(x), toReal3(y));
    }

    const auto finish = QDateTime::currentMSecsSinceEpoch();
    emit message("update blobs finished");
    emit message("time " + QString::number(std::floor((finish-start)/1000)) + " sec");
    emit message("count " + QString::number(count));
}
