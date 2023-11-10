#include "task_test_algo.h"
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


#include "common.h"


TaskTestAlgo::TaskTestAlgo(QObject* parent)
    : QObject(parent)
    , _impl(new TaskTestAlgoPrivate)
    , _thread(new QThread)
{
    connect(_impl, &TaskTestAlgoPrivate::message, this, &TaskTestAlgo::message, Qt::QueuedConnection);
    connect(_impl, &TaskTestAlgoPrivate::finished, this, &TaskTestAlgo::finished, Qt::QueuedConnection);

    connect(_thread.data(), &QThread::finished, _impl, &QObject::deleteLater);

    _impl->moveToThread(_thread.data());
    _thread->start();
}

TaskTestAlgo::~TaskTestAlgo()
{
    _thread->quit();
    _thread->wait(1000);
}

void TaskTestAlgo::run()
{
    _impl->_stop = false;
    QMetaObject::invokeMethod(_impl, "run", Qt::QueuedConnection);
}

void TaskTestAlgo::stopProgram()
{
    _impl->_stop = true;

}


TaskTestAlgoPrivate::TaskTestAlgoPrivate()
{

}

void TaskTestAlgoPrivate::run()
{
    if (!_mutex.tryLock()) return;
    auto mutexUnlock = qScopeGuard([this]{ _mutex.unlock(); });

    db().insert("step", "");

    while(true)
    {
        if (_stop)
        {
            emit message("program interrupted");
            break;
        }

        scene().clear();
        scene().addBoard();


        waitDataBus("step", "next"); db().insert("step", "");


        QList<std::tuple<BlobItem*, BlobItem*>> fiducialBlobs; // Пары опорных точек - идеальная и реальная

        { BlobItem* bl1 = scene().addBlob(10,10,2);
          bl1->setFiducial(true);
          BlobItem* bl2 = scene().addBlob(10,13,2, true);
          bl2->setRealFiducial(true);
          fiducialBlobs.append(std::make_tuple(bl1, bl2));
        }

        { BlobItem* bl1 = scene().addBlob(1,10,2);
          bl1->setFiducial(true);
          BlobItem* bl2 = scene().addBlob(1,13,2, true);
          bl2->setRealFiducial(true);
          fiducialBlobs.append(std::make_tuple(bl1, bl2));
        }

        waitDataBus("step", "next"); db().insert("step", "");

        if (fiducialBlobs.size() != 2)
            return;

        QPointF firstRef = std::get<0>(fiducialBlobs[0])->scenePos();
        QPointF firstReal = std::get<1>(fiducialBlobs[0])->scenePos();

        BlobItem* secondRef = std::get<0>(fiducialBlobs[1]);
        BlobItem* secondReal = std::get<1>(fiducialBlobs[1]);

        algorithmMatchPoints(firstRef, firstReal, secondRef, secondReal);
    }

    emit finished();
}
