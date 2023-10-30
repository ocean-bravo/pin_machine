#include "update_blobs.h"
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

//QStringList removeDuplicatedBlobs(const QStringList& blobs)
//{
//    //ScopedMeasure("remove duplicated blobs");

//    QGraphicsScene scene;

//    // Отправляю все блобы на сцену
//    for (const QString& blob : blobs)
//    {
//        auto [x, y, dia] = blobToDouble(blob);

//        QGraphicsEllipseItem* item = scene.addEllipse(-dia/2, -dia/2, dia, dia);
//        item->setPos(x, y);
//    }

//    // если есть пересечение с кем то, то удалить его
//    for (QGraphicsItem* item : qAsConst(scene).items())
//    {
//        if (!scene.collidingItems(item).isEmpty())
//            delete item;
//    }

//    QStringList b;
//    for (const QGraphicsItem* item : qAsConst(scene).items())
//    {
//        const QGraphicsEllipseItem* blob = dynamic_cast<const QGraphicsEllipseItem*>(item);

//        b.append(toReal3(blob->x()) + " " + toReal3(blob->y()) + " " + toReal3(blob->rect().width()));
//    }

//    return b;
//}

}

UpdateBlobs::UpdateBlobs(Video4 *video, QObject* parent)
    : QObject(parent)
    , _impl(new UpdateBlobsPrivate(video))
    , _thread(new QThread)
{
    connect(_impl, &UpdateBlobsPrivate::message, this, &UpdateBlobs::message, Qt::QueuedConnection);
    connect(_impl, &UpdateBlobsPrivate::finished, this, &UpdateBlobs::finished, Qt::QueuedConnection);

    connect(_thread.data(), &QThread::finished, _impl, &QObject::deleteLater);

    _impl->moveToThread(_thread.data());
    _thread->start();
}

UpdateBlobs::~UpdateBlobs()
{
    _thread->quit();
    _thread->wait(1000);
}

void UpdateBlobs::run()
{
    _impl->_stop = false;
    QMetaObject::invokeMethod(_impl, "run", Qt::QueuedConnection);
}

void UpdateBlobs::stopProgram()
{
    _impl->_stop = true;
}


UpdateBlobsPrivate::UpdateBlobsPrivate(Video4 *video)
    : _video(video)
{

}

void UpdateBlobsPrivate::pauseProgram()
{

}

//void UpdateBlobsPrivate::waitForGetPosition(double xTarget, double yTarget)
//{
//    auto condition = [xTarget, yTarget]() -> bool
//    {
//        const QString status = db().value("status").toString();
//        const double xPos = db().value("xPos").toDouble();
//        const double yPos = db().value("yPos").toDouble();

//        return (status == "Idle") && (std::abs(xTarget - xPos) <= 0.003) && (std::abs(yTarget - yPos) <= 0.003);
//    };

//    QEventLoop loop;
//    QTimer::singleShot(2000, &loop, &QEventLoop::quit);

//    QMetaObject::Connection conn = connect(&db(), &DataBus::valueChanged, this, [&condition, &loop](const QString& key, const QVariant&)
//    {
//        if ( key == "status" || key == "xPos" || key == "yPos")
//        {
//            if (condition())
//                loop.quit();
//        }
//    });

//    auto guard = qScopeGuard([=]()
//    {
//        disconnect(conn);
//    });

//    // у машины есть зона нечувствительности, т.е. она не реагирует на микроперемещения, около 0,002 мм
//    // При уточнении блоба машина просто не поедет никуда, хотя ей задали двигаться. И мы зависнем на таймаут в этой функции
//    if (condition())
//        return;

//    loop.exec();
//}

void UpdateBlobsPrivate::wait(int timeout) const
{
    if (timeout <= 0)
        return;

    waitForSignal(this, &UpdateBlobsPrivate::interrupt, timeout);
}

void UpdateBlobsPrivate::run()
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
    auto updateBlobPosition = [this, &moveTo] (BlobItem* blob) -> bool
    {
        double xTarget = blob->x();
        double yTarget = blob->y();

        moveTo(xTarget, yTarget);

        waitForGetPosition(xTarget, yTarget);

        emit message("capturing ...");

        _video->captureSmallRegion(5.5);

        waitForSignal(_video, &Video4::capturedSmallRegion, 2000);

        emit message("captured");

        QImage smallRegion = _video->smallRegion();
        opencv().blobDetectorUpdated(smallRegion);

        waitForSignal(&opencv(), &OpenCv::smallRegionBlobChanged, 500);

        auto [ok, x, y, dia] = opencv().smallRegionBlob();

        if (!ok)
        {
            emit message("blob NOT found");
            // Как то пометить блоб, что он не найден в этом месте
            return false;
        }
        else
        {
            emit message("blob found");
            qd() << "blob found";

            // Обновили позицию и диаметро блоба
            scene().updateBlob(blob, x, y, dia);
            return true;
        }
    };

    const auto start = QDateTime::currentMSecsSinceEpoch();

    scene().removeDuplicatedBlobs();

//    auto connection = connect(_video, &Video4::capturedSmallRegion, this, [](QImage img) { scene().setImage(img); });
    auto connection = connect(_video, &Video4::capturedSmallRegion, this, [](QImage img) {
            QMetaObject::invokeMethod(&scene(), "setImage", Qt::QueuedConnection, Q_ARG(QImage, img));
    });
    auto guard = qScopeGuard([=]() { disconnect(connection); });

    int count  = 0;
    for (QGraphicsItem* item  : scene().items())
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
        bool ok2 = updateBlobPosition(blob);

        if (ok2)
        {
//            auto dist = distance(blob, foundPoint2);
//            updatedBlobs.push(foundPoint2 + " " + dist);
        }
        else
        {
            //updatedBlobs.push(blob += " NOK")
        }
    }

    const auto finish = QDateTime::currentMSecsSinceEpoch();
    emit message("update blobs finished");
    emit message("time " + QString::number(std::floor((finish-start)/1000)) + " sec");
    emit message("count " + QString::number(count));

    emit finished();
}

//void UpdateBlobsPrivate::run2()
//{
//    stopProgram = false;

//    QStringList blobs = db().value("found_blobs3").toStringList();

//    if (blobs.isEmpty()) {
//        emit message("no blobs to visit");
//        return;
//    }

//    blobs = removeDuplicatedBlobs(blobs);

//    QStringList updatedBlobs;

//    QTimer statusTimer;
//    connect(&statusTimer, &QTimer::timeout, this, []() { serial().write("?\n"); });
//    statusTimer.start(100);

//    auto moveTo = [](double x, double y)
//    {
//        const QString line = QString("G1 G90 F5000 X%1 Y%2").arg(toReal(x), toReal(y));
//        serial().write(line.toLatin1() + "\n");
//    };

//    // point - массив строк. Возвр значение строка с пробелом между координатами
//    auto updateBlobPosition = [&] (double xTarget, double yTarget, double dia) -> std::tuple<bool, double, double, double>
//    {
//        moveTo(xTarget, yTarget);

//        waitForGetPosition(xTarget, yTarget);

//        emit message("capturing ...");

//        _video->captureSmallRegion(5.5);

//        waitForSignal(_video, &Video4::capturedSmallRegion, 2000);

//        emit message("captured");

//        auto smallRegion = _video->smallRegion();
//        opencv().blobDetectorUpdated(smallRegion);

//        waitForSignal(&opencv(), &OpenCv::smallRegionBlobChanged, 2000);

//        const QString coordBlob = opencv().smallRegionBlob();

//        if (coordBlob.isEmpty())
//        {
//            emit message("blob NOT found");
//            return {false, xTarget, yTarget, dia}; // диаметр прокидывается насквозь. подумать
//        }
//        else
//        {
//            emit message("blob found");
//            auto [x, y, dia] = blobToDouble(coordBlob);
//            return {true, x, y, dia};
//        }
//    };

//        // Может взять все блобы со сцены и посетить их?

//    const auto start = QDateTime::currentMSecsSinceEpoch();

//    for (const QString& blob : qAsConst(blobs))
//    {
//        if (stopProgram)
//        {
//            emit message("program interrupted");
//            break;
//        }


//        auto [x, y, dia] = blobToDouble(blob);

//        auto [ok1, x1, y1, dia1] = updateBlobPosition(x, y, dia);

//        auto [ok2, x2, y2, dia2] = updateBlobPosition(x1, y1, dia1);

//        if (ok2)
//        {
////            auto dist = distance(blob, foundPoint2);
////            updatedBlobs.push(foundPoint2 + " " + dist);
//        }
//        else
//        {
//            //updatedBlobs.push(blob += " NOK")
//        }

//        updatedBlobs.append(QString("%1 %2 %3").arg(x2).arg(y2).arg(dia2));
//    }

//    const auto finish = QDateTime::currentMSecsSinceEpoch();
//    emit message("update blobs finished");
//    emit message("time " + QString::number(std::floor((finish-start)/1000)) + " sec");
//    emit message("count " + QString::number(updatedBlobs.size()));

//    db().insert("found_blobs3", updatedBlobs);
//}
