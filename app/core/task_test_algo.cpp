#include "task_test_algo.h"
#include "utils2.h"
#include "data_bus.h"
#include "scene.h"
#include "wait.h"
#include "blob_item.h"
#include "common.h"

#include <QScopeGuard>

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
    const auto fin = qScopeGuard([this]{ emit finished(); });

    if (!_someTaskInProgress.tryLock()) return;
    auto mutexUnlock = qScopeGuard([this]{ _someTaskInProgress.unlock(); });

    scene().clear();
    scene().addBoard();

    while(true)
    {
        if (_stop)
        {
            emit message("program interrupted");
            break;
        }

        waitForNext();

        runOnThreadWait(&scene(), []() { scene().board()->setTransformOriginPoint({0,0});});

        waitForNext();

        runOnThreadWait(&scene(), []() { scene().board()->setRotation(0);});
        runOnThreadWait(&scene(), []() { scene().board()->setPos({0,0});});

        every<BlobItem>(scene().items(), [](BlobItem* blob)
        {
            if (blob->isFiducial() || blob->isRealFiducial())
                delete blob;
        });

        every<BlobItem>(scene().board()->childItems(), [](BlobItem* blob)
        {
            if (blob->isFiducial())
                blob->deleteLater();

            if (blob->isRealFiducial())
                blob->deleteLater();
        });

        wait(100);

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

        waitForNext();

        if (fiducialBlobs.size() != 2)
            return;

        QPointF firstRef = std::get<0>(fiducialBlobs[0])->scenePos();
        QPointF firstReal = std::get<1>(fiducialBlobs[0])->scenePos();

        BlobItem* secondRef = std::get<0>(fiducialBlobs[1]);
        BlobItem* secondReal = std::get<1>(fiducialBlobs[1]);

        algorithmMatchPoints(firstRef, firstReal, secondRef, secondReal);
    }
}
