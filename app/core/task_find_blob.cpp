#include "task_find_blob.h"
#include "wait.h"
#include "video4.h"
#include "serial.h"
#include "scene.h"
#include "utils2.h"
#include "data_bus.h"
#include "openCv.h"
#include "find_blob_view_item.h"
#include "common.h"

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
    _impl->_stopObj.run();
    QMetaObject::invokeMethod(_impl, "run", Qt::QueuedConnection, Q_ARG(QVariantMap, options), Q_ARG(bool, slow));
}

void TaskFindBlob::stopProgram()
{
    _impl->_stop = true;
    _impl->_stopObj.stop();
}


TaskFindBlobPrivate::TaskFindBlobPrivate()
{

}

void TaskFindBlobPrivate::run(QVariantMap options, bool slow)
{
    const auto fin = qScopeGuard([this]{ emit finished(); });

    if (!_someTaskInProgress.tryLock()) return;
    auto mutexUnlock = qScopeGuard([this]{ _someTaskInProgress.unlock(); });

    const auto start = QDateTime::currentMSecsSinceEpoch();

    try
    {
        QList<QGraphicsPixmapItem*> pixs = scene().pixmaps();

        // подать на обнаружение
        qd() << "images to find " << pixs.size();

        // every<FindBlobViewItem>(scene().items(), [](FindBlobViewItem* item) { delete item; });
        // FindBlobViewItem* viewItem = new FindBlobViewItem;
        // scene().addItem(viewItem);

        for (QGraphicsPixmapItem* pixmap : pixs)
        {
            QImage img = pixmap->pixmap().toImage().convertToFormat(QImage::Format_RGB888, Qt::ColorOnly);

            // auto w = img.width();
            // auto h = img.height();
            // auto ps = img.devicePixelRatioF();
            // auto x = img.text("x").toDouble();
            // auto y = img.text("y").toDouble();

            // viewItem->setPos(x, y);
            // viewItem->setRect(-w/(ps*2), -h/(ps*2), w/ps, h/ps);

            db().insert("image_raw_captured", img.copy());
            opencv().appendToBlobDetectorQueue(img.mirrored(false, true), options);
            //wait(10); //(slow ? 2000 : 10);
        }

        if (_stop)
            throw stopEx();

        // int exit = waitForSignals(100000, a, &OpenCv::queueIsEmpty); //, &_stopObj, &Stop::stopped);
        int exit = 0;
        {
            int timeout = 100000;
            QEventLoop loop;
            QTimer::singleShot(timeout, &loop, [&loop]() { loop.quit(); });
            QMetaObject::Connection conn1 = QObject::connect(&opencv(), &OpenCv::queueIsEmpty, &loop, [&loop, &exit]() { exit = 1; loop.quit(); });
            QMetaObject::Connection conn2 = QObject::connect(&_stopObj, &Stop::stopped,        &loop, [&loop, &exit]() { exit = 2; loop.quit(); });

            auto guard = qScopeGuard([=]()
            {
                QObject::disconnect(conn1);
                QObject::disconnect(conn2);
            });
            loop.exec();
        }

        if (exit == 2)
            throw stopEx();

        {
            qd() << "start remove blobs";
            ScopedMeasure m("remove blobs", ScopedMeasure::Milli);
            scene().removeDuplicatedBlobs();
        }
    }
    catch (const stopEx& e)
    {
        qd() << "program interrupted";
        emit message("program interrupted");
        opencv().clearQueue();
    }
    catch (...)
    {

    }

    const auto finish = QDateTime::currentMSecsSinceEpoch();
    emit message("find blobs finished");
    emit message("time " + QString::number(std::floor((finish-start)/1000)) + " sec");
}
