#include "task_punch.h"
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

TaskPunch::TaskPunch(QObject* parent)
    : QObject(parent)
    , _impl(new TaskPunchPrivate)
    , _thread(new QThread)
{
    connect(_impl, &TaskPunchPrivate::message, this, &TaskPunch::message, Qt::QueuedConnection);
    connect(_impl, &TaskPunchPrivate::finished, this, &TaskPunch::finished, Qt::QueuedConnection);

    connect(_thread.data(), &QThread::finished, _impl, &QObject::deleteLater);

    _impl->moveToThread(_thread.data());
    _thread->start();
}

TaskPunch::~TaskPunch()
{
    _thread->quit();
    _thread->wait(1000);
}

void TaskPunch::run()
{
    _impl->_stop = false;
    QMetaObject::invokeMethod(_impl, "run", Qt::QueuedConnection);
}

void TaskPunch::stopProgram()
{
    _impl->_stop = true;
}


TaskPunchPrivate::TaskPunchPrivate()
{

}

void TaskPunchPrivate::run()
{
    // Разница в диаметрах блобов, больше которой считается что блоб неправильный
    static const double wrongBlobDiaError = 0.3;

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

    auto connection = connect(&video(), &Video4::capturedSmallRegion, &scene(), &Scene::setImage);
    auto guard = qScopeGuard([=]() { disconnect(connection); });


    QList<BlobItem*> referenceFiducialBlobs;

    every<BlobItem>(scene().items(), [&referenceFiducialBlobs](BlobItem* blob)
    {
        if (blob->isFiducial())
            referenceFiducialBlobs.append(blob);
    });

    int count  = 0;
    QList<std::tuple<BlobItem*, BlobItem*>> fiducialBlobs; // Пары опорных точек - идеальная и реальная
    for (BlobItem* referenceFiducialBlob  : qAsConst(referenceFiducialBlobs))
    {
        if (_stop)
        {
            emit message("program interrupted");
            break;
        }

        ++count;

        BlobItem* realFiducialBlob = scene().addBlobCopy(referenceFiducialBlob);
        realFiducialBlob->setFiducial(true);
        realFiducialBlob->setRotation(45); // чтобы его отличать от идеальной опорной точки

        updateBlobPosition(realFiducialBlob);
        int result = updateBlobPosition(realFiducialBlob);
        if (result > 0)
        {
            result = updateBlobPosition(realFiducialBlob);
            if (result > 0)
                result = updateBlobPosition(realFiducialBlob);
        }
        fiducialBlobs.append(std::make_tuple(referenceFiducialBlob, realFiducialBlob));
    }

    // Теперь надо совместить каждую пару referenceFiducialBlob и realFiducialBlob.
    // В идеале, таких пар должно быть 2. Меньше вообще нельзя, а больше смысла не имеет, только сложнее расчет поворота платы.
    // referenceFiducialBlob привязана к контуру платы (имеет его в качестве родителя).
    // realFiducialBlob  не привязана никуда.

    // Получаем плату со сцены
    QGraphicsItem* board = scene().board();

    QPointF firstRef = std::get<0>(fiducialBlobs[0])->pos();
    QPointF firstReal = std::get<1>(fiducialBlobs[0])->pos();

    QPointF secondRef = std::get<0>(fiducialBlobs[1])->pos();
    QPointF secondReal = std::get<1>(fiducialBlobs[1])->pos();

    // Помещаем transform origin всей платы в первую идеальную fid точку.
    board->setTransformOriginPoint(firstRef);

    // Передвигаем плату в 1-ю реальную fid точку. 1-ые реальная и идеальная точки совпали.
    board->setPos(firstReal);

    double angleReal = QLineF(firstReal, secondReal).angle();
    double angleRef = QLineF(firstRef, secondRef).angle();

    double deltaAngle = angleReal - angleRef;

    // Довернули плату до реального угла
    board->setRotation(deltaAngle); // TODO: По тестам определить знак угла





    // Двигаем плату до совпадения с реальной fid точки.
    // Получается надо иметь пары - идеальная fid точка и реальная fid точки.
    // дальше определяем угол между реальными fid точками и доворачиваем плату до этого угла
    // Чтобы идеальные fid точки имели этот же угол.




    const auto finish = QDateTime::currentMSecsSinceEpoch();
    emit message("punch blobs finished");
    emit message("time " + QString::number(std::floor((finish-start)/1000)) + " sec");
    emit message("count " + QString::number(count));

    emit finished();
}
