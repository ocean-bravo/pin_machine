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

    auto moveTo = [](double x, double y)
    {
        const QString line = QString("G1 G90 F5000 X%1 Y%2").arg(toReal3(x), toReal3(y));
        serial().write(line.toLatin1() + "\n");
    };

    // point - массив строк. Возвр значение строка с пробелом между координатами
    auto updateBlobPosition = [this, &moveTo] (BlobItem* blob) -> int
    {
        double xTarget = blob->x();
        double yTarget = blob->y();
        double diaTarget = blob->rect().width();

        moveTo(xTarget, yTarget);

        waitForGetPosition(xTarget, yTarget);

        emit message("capturing ...");

        _video->captureSmallRegion(diaTarget + 2);

        waitForSignal(_video, &Video4::capturedSmallRegion, 2000);

        emit message("captured");

        QImage smallRegion = _video->smallRegion();
        opencv().blobDetectorUpdated(smallRegion);

        waitForSignal(&opencv(), &OpenCv::smallRegionBlobChanged, 500);

        auto [ok, x, y, dia] = opencv().smallRegionBlob();

        if (ok && (dia / diaTarget > 2)) // неправильный блоб
            return 2;

        //qd() << "diameter " << dia;
        if (!ok)
        {
            emit message("blob NOT found");
            // Как то пометить блоб, что он не найден в этом месте
            return 1;
        }
        else
        {
            emit message("blob found");
            qd() << "blob found";

            // Обновили позицию и диаметро блоба
            scene().updateBlob(blob, x, y, dia);
            return 0;
        }
    };

    const auto start = QDateTime::currentMSecsSinceEpoch();

    scene().removeDuplicatedBlobs();

    auto connection = connect(_video, &Video4::capturedSmallRegion, &scene(), &Scene::setImage);
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
