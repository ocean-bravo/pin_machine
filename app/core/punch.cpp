#include "punch.h"
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

Punch::Punch(Video4 *video, QObject* parent)
    : QObject(parent)
    , _impl(new PunchPrivate(video))
    , _thread(new QThread)
{
    connect(_impl, &PunchPrivate::message, this, &Punch::message, Qt::QueuedConnection);
    connect(_impl, &PunchPrivate::finished, this, &Punch::finished, Qt::QueuedConnection);

    connect(_thread.data(), &QThread::finished, _impl, &QObject::deleteLater);

    _impl->moveToThread(_thread.data());
    _thread->start();
}

Punch::~Punch()
{
    _thread->quit();
    _thread->wait(1000);
}

void Punch::run()
{
    _impl->_stop = false;
    QMetaObject::invokeMethod(_impl, "run", Qt::QueuedConnection);
}

void Punch::stopProgram()
{
    _impl->_stop = true;
}


PunchPrivate::PunchPrivate(Video4 *video)
    : _video(video)
{

}

void PunchPrivate::run()
{
    // Разница в диаметрах блобов, больше которой считается что блоб неправильный
    static const double wrongBlobDiaError = 0.3;

    if (!_mutex.tryLock()) return;
    auto mutexUnlock = qScopeGuard([this]{ _mutex.unlock(); });

    _video->stop();

    db().insert("resolution_width", 1280);
    db().insert("resolution_height", 960);
    db().insert("pixel_size", 0.0107);

    _video->changeCamera(0, 1280, 960, "YUYV"); // НУжен номер девайса
    _video->start();

    QTimer statusTimer;
    connect(&statusTimer, &QTimer::timeout, this, []() { serial().write("?\n"); });
    statusTimer.start(100);

    const auto start = QDateTime::currentMSecsSinceEpoch();

    auto connection = connect(_video, &Video4::capturedSmallRegion, &scene(), &Scene::setImage);
    auto guard = qScopeGuard([=]() { disconnect(connection); });


    QList<BlobItem*> referenceBlobs;

    every<BlobItem>(scene().items(), [&referenceBlobs](BlobItem* blob)
    {
        if (blob->isFiducial())
            referenceBlobs.append(blob);
    });

    int count  = 0;
    for (BlobItem* refBlob  : qAsConst(referenceBlobs))
    {
        if (_stop)
        {
            emit message("program interrupted");
            break;
        }

        ++count;

        BlobItem* workBlob = scene().addBlobCopy(refBlob);
        workBlob->setWork(true);

        updateBlobPosition(workBlob);
        int result = updateBlobPosition(workBlob);
        if (result > 0)
        {
            result = updateBlobPosition(workBlob);
            if (result > 0)
                result = updateBlobPosition(workBlob);
        }
    }

    const auto finish = QDateTime::currentMSecsSinceEpoch();
    emit message("punch blobs finished");
    emit message("time " + QString::number(std::floor((finish-start)/1000)) + " sec");
    emit message("count " + QString::number(count));

    emit finished();
}
