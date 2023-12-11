#include "task_check_pixel_size.h"
#include "wait.h"
#include "video4.h"
#include "serial.h"
#include "utils.h"
#include "utils2.h"
#include "data_bus.h"
#include "scene.h"
#include "graphics_view.h"

#include <QEventLoop>
#include <QTimer>
#include <QMetaMethod>
#include <QScopeGuard>
#include <QDateTime>

namespace {

}

TaskCheckPixelSize::TaskCheckPixelSize(QObject* parent)
    : QObject(parent)
    , _impl(new TaskCheckPixelSizePrivate)
    , _thread(new QThread)
{
    connect(_impl, &TaskCheckPixelSizePrivate::message, this, &TaskCheckPixelSize::message, Qt::QueuedConnection);
    connect(_impl, &TaskCheckPixelSizePrivate::finished, this, &TaskCheckPixelSize::finished, Qt::QueuedConnection);

    connect(_thread.data(), &QThread::finished, _impl, &QObject::deleteLater);

    _impl->moveToThread(_thread.data());
    _thread->start();
}

TaskCheckPixelSize::~TaskCheckPixelSize()
{
    _thread->quit();
    _thread->wait(1000);
}

void TaskCheckPixelSize::run(int width, int height, QString fourcc)
{
    _impl->_stop = false;
    QMetaObject::invokeMethod(_impl, "run", Qt::QueuedConnection, Q_ARG(int, width), Q_ARG(int, height), Q_ARG(QString, fourcc));
}

void TaskCheckPixelSize::stopProgram()
{
    _impl->_stop = true;
}

TaskCheckPixelSizePrivate::TaskCheckPixelSizePrivate()
{

}

void TaskCheckPixelSizePrivate::run(int width, int height, QString fourcc)
{
    if (!_mutex.tryLock()) return;
    auto mutexUnlock = qScopeGuard([this]{ _mutex.unlock(); });

    // Мешается GUI. При обнуражении камеры идет ее запуск. Решить бы это как то.
    video().reloadDevices();
    wait(500);
    video().stop();

    db().insert("resolution_width", width);
    db().insert("resolution_height", height);

    video().changeCamera(cameraId(), width, height, fourcc);
    video().start();

    QTimer statusTimer;
    connect(&statusTimer, &QTimer::timeout, this, []() { serial().write("?\n"); });
    statusTimer.start(100);

    scene().clear();
    scene().addBoard();

    // Костыль. Чтобы плата показывалась во весь экран. Иначе плата была неизвестно где, на сцене ее было не видно.
    for (QGraphicsView* view : scene().views())
    {
        GraphicsView* v = dynamic_cast<GraphicsView*>(view);
        if (v)
            v->fit();
    }

    auto start = QDateTime::currentMSecsSinceEpoch();

    auto connection1 = connect(&video(), &Video4::captured, &scene(), &Scene::setImage);
    auto guard1 = qScopeGuard([=]() { disconnect(connection1); });

    auto connection2 = connect(&video(), &Video4::capturedSmallRegion, &scene(), &Scene::setImage);
    auto guard2 = qScopeGuard([=]() { disconnect(connection2); });

    double dia = 3;
    //for (int i = 0; i < 10; ++i)
    {
        if (_stop)
        {
            emit message("program interrupted");
            //break;
        }

        const double xCurrent = db().value("xPos").toDouble();
        const double yCurrent = db().value("yPos").toDouble();

        video().capture();
        waitForSignal(&video(), &Video4::captured, 10000);
        wait(5000);

        moveToAndWaitPosition(xCurrent + 3, yCurrent + 3);
        moveToAndWaitPosition(xCurrent, yCurrent);
        wait(1000);
        video().capture(dia);
        waitForSignal(&video(), &Video4::capturedSmallRegion, 10000);

        wait(1000);
        moveToAndWaitPosition(xCurrent - 3, yCurrent - 3);
        moveToAndWaitPosition(xCurrent, yCurrent);
        wait(1000);
        video().capture(dia);
        waitForSignal(&video(), &Video4::capturedSmallRegion, 10000);


        wait(1000);
        moveToAndWaitPosition(xCurrent + 3, yCurrent - 3);
        moveToAndWaitPosition(xCurrent, yCurrent);
        wait(1000);
        video().capture(dia);
        waitForSignal(&video(), &Video4::capturedSmallRegion, 10000);

        wait(1000);
        moveToAndWaitPosition(xCurrent - 3, yCurrent + 3);
        moveToAndWaitPosition(xCurrent, yCurrent);
        wait(1000);
        video().capture(dia);
        waitForSignal(&video(), &Video4::capturedSmallRegion, 10000);

        dia -= 0.5;

//        const double xCurrent = db().value("xPos").toDouble();
//        const double yCurrent = db().value("yPos").toDouble();

//        moveToAndWaitPosition(xCurrent + 5.0, yCurrent + 0.5);

//        emit message("capturing ...");
//        auto a = QDateTime::currentMSecsSinceEpoch();
//        video().capture();
//        waitForSignal(&video(), &Video4::captured, 2000);
//        auto b = QDateTime::currentMSecsSinceEpoch();
//        emit message(QString("captured %1 ms").arg(b-a));
    }

    auto finish = QDateTime::currentMSecsSinceEpoch();

    emit message("program time " + QString::number((finish - start)/1000) + " sec");
    wait(10);

    emit finished();
}
