#include "task_find_pixel_size.h"
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
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>

#include "openCv.h"
#include "settings.h"


namespace {

}

TaskFindPixelSize::TaskFindPixelSize(QObject* parent)
    : QObject(parent)
    , _impl(new TaskFindPixelSizePrivate)
    , _thread(new QThread)
{
    connect(_impl, &TaskFindPixelSizePrivate::message, this, &TaskFindPixelSize::message, Qt::QueuedConnection);
    connect(_impl, &TaskFindPixelSizePrivate::finished, this, &TaskFindPixelSize::finished, Qt::QueuedConnection);

    connect(_thread.data(), &QThread::finished, _impl, &QObject::deleteLater);

    _impl->moveToThread(_thread.data());
    _thread->start();
}

TaskFindPixelSize::~TaskFindPixelSize()
{
    _thread->quit();
    _thread->wait(1000);
}

void TaskFindPixelSize::run()
{
    _impl->_stop = false;
    QMetaObject::invokeMethod(_impl, "run", Qt::QueuedConnection);
}

void TaskFindPixelSize::stopProgram()
{
    _impl->_stop = true;
}

TaskFindPixelSizePrivate::TaskFindPixelSizePrivate()
{

}

// Скан может быть неудачным для какого то разрешения. Определить, и предупредить
void TaskFindPixelSizePrivate::run()
{
    const auto fin = qScopeGuard([this]{ emit finished(); });

    if (!_mutex.tryLock()) return;
    auto mutexUnlock = qScopeGuard([this]{ _mutex.unlock(); });

    QTimer statusTimer;
    connect(&statusTimer, &QTimer::timeout, this, []() { serial().write("?\n"); });
    statusTimer.start(100);

    const auto start = QDateTime::currentMSecsSinceEpoch();

    // Мешается GUI. При обнуражении камеры идет ее запуск. Решить бы это как то.
    video().reloadDevices();
    wait(500);
    video().stop();

    QVector<int> widths = uniqueWidths();

    // 1 стадия
    QMap<int, QImage> stage1;
    moveToAndWaitPosition(currPos().x() + 3, currPos().y());

    for (int i = 0; i < widths.size(); ++i)
    {
        const int width = widths[i];
        const int height = anyHeightForWidth(width);
        const QString fourcc = anyFourcc(width, height);

        video().stop();
        db().insert("resolution_width", width);
        db().insert("resolution_height", height);
        video().changeCamera(cameraId(), width, height, fourcc);
        video().start();

        wait(1000); // чтобы изображение подстроилось

        auto connection = connect(&video(), &Video4::captured, [width, &stage1](QImage img)
        {
            stage1.insert(width, img.copy());
        });
        auto guard = qScopeGuard([=]() { disconnect(connection); });
        video().capture();
        waitForSignal(&video(), &Video4::captured, 10000);
    }

    // 2 стадия
    QMap<int, QImage> stage2;
    moveToAndWaitPosition(currPos().x() - 6, currPos().y());

    for (int i = 0; i < widths.size(); ++i)
    {
        const int width = widths[i];
        const int height = anyHeightForWidth(width);
        const QString fourcc = anyFourcc(width, height);

        video().stop();
        db().insert("resolution_width", width);
        db().insert("resolution_height", height);
        video().changeCamera(cameraId(), width, height, fourcc);
        video().start();

        wait(1000); // чтобы изображение подстроилось

        auto connection = connect(&video(), &Video4::captured, [width, &stage2](QImage img)
        {
            stage2.insert(width, img.copy());
        });
        auto guard = qScopeGuard([=]() { disconnect(connection); });
        video().capture();
        waitForSignal(&video(), &Video4::captured, 10000);
    }

    // 3 стадия
    for (int i = 0; i < widths.size(); ++i)
    {
        const int width = widths[i];
        double dist = OpenCv::corr(stage1[width], stage2[width]);

        settings().setValue("pixel_size_" + QString::number(width), dist / 6.0);
        db().insert("pixel_size_" + QString::number(width), dist / 6.0);
    }

    auto finish = QDateTime::currentMSecsSinceEpoch();

    emit message("program time " + QString::number((finish - start)/1000) + " sec");
    wait(10);
}
