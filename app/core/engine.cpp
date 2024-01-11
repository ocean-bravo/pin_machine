#include "engine.h"

#include "serial.h"
#include "version.h" // автогенерированный файл qmake из version.h.in
#include "utils.h"
#include "utils2.h"
#include "wait.h"

#include <QCoreApplication>
#include <QQmlContext>
#include <QApplication>
#include <QQmlApplicationEngine>
#include <QProcess>
#include <QBuffer>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QSplashScreen>

#include <QQuickStyle>
#include <QScopeGuard>

#include "data_bus.h"
#include "task_scan.h"
#include "task_update.h"
#include "task_test_scan_update_cycle.h"
#include "task_test_algo.h"
#include "task_check_camera.h"
#include "task_punch.h"
#include "task_check_pixel_size.h"

#include "openCv.h"

#include "my_image_provider.h"

#include "scene.h"
#include "settings.h"

#include "ImageItem.h"


Engine::Engine(QObject* parent)
    : QObject(parent)
    , _qmlEngine(nullptr)
{
    Settings::instance();
    Serial::instance();
    DataBus::instance();
    OpenCv::instance();

    //_videoDriver3 = new Video3();
    Video4::instance();

    createQmlEngine();
}


//QStringList Engine::camerasInfo()
//{
//    return _info;
//}

void Engine::save(const QString& url)
{
    QSplashScreen splash(QPixmap("./splash.png"), Qt::WindowType(Qt::SplashScreen + Qt::FramelessWindowHint + Qt::WindowStaysOnTopHint));
    splash.setEnabled(false);
    splash.setWindowModality(Qt::ApplicationModal);
    splash.show();

    int count = scene().images();

    auto connection = connect(&scene(), &Scene::imageSaved, this, [&splash, count](int i)
    {
        splash.showMessage(QString("Saved %1 of %2 images").arg(i).arg(count));
    });

    auto guard = qScopeGuard([=]() { disconnect(connection); });

    scene().saveScene(url);
}

void Engine::load(const QString& url)
{
    QSplashScreen splash(QPixmap("./splash.png"), Qt::WindowType(Qt::SplashScreen + Qt::FramelessWindowHint + Qt::WindowStaysOnTopHint));
    splash.setEnabled(false);
    splash.setWindowModality(Qt::ApplicationModal);
    splash.show();

    scene().loadScene(url);

//    QList<QGraphicsView *> views = scene().views();

//    for (QGraphicsView * view : views)
//    {
//        QMetaObject::invokeMethod(view, "fit", Qt::QueuedConnection);
    //    }
}

// void Engine::capture1()
// {
//     auto connection = connect(&video(), &Video4::captured, [](QImage img)
//     {
//         db().insert("capture1", img.copy());
//     });
//     auto guard = qScopeGuard([=]() { disconnect(connection); });
//     video().start();
//     video().capture();
//     waitForSignal(&video(), &Video4::captured, 2000);
// }

// void Engine::capture2()
// {
//     auto connection = connect(&video(), &Video4::captured, [](QImage img)
//     {
//         db().insert("capture2", img.copy());
//     });
//     auto guard = qScopeGuard([=]() { disconnect(connection); });
//     video().start();
//     video().capture();
//     waitForSignal(&video(), &Video4::captured, 2000);
// }

// void Engine::corr()
// {
//     opencv().corr();
// }

void Engine::capture()
{
    auto connection = connect(&video(), &Video4::captured, &scene(), &Scene::setImage);
    auto guard = qScopeGuard([=]() { disconnect(connection); });
    video().start();
    video().capture();
    waitForSignal(&video(), &Video4::captured, 2000);
}

Engine::~Engine()
{
    _qmlEngine.reset();
}

void Engine::createQmlEngine()
{
    connect(&video(), &Video4::rawImage, this, [](QImage img)
    {
        const QString mode = db().value("mode").toString();

        if (mode == "raw")
            db().insert("image_raw", img.copy());

        if (mode == "circle")
            opencv().searchCirclesLive(img.copy());

        if (mode == "blob" || mode == "adapt_threshold_1" || mode == "adapt_threshold_2")
            opencv().blobDetectorLive(img.copy());
    });

    connect(&video(), &Video4::capturedSmallRegion, this, [](QImage img)
    {
        db().insert("image_raw_captured", img.copy());
    });

    connect(&video(), &Video4::captured, this, [](QImage img)
    {
        db().insert("image_raw_captured", img.copy());
        opencv().blobDetectorCaptured(img.copy());
    });

    connect(&opencv(), &OpenCv::circleChanged, this, [](QImage img)
    {
        db().insert("image_circle", img);
    });

    connect(&opencv(), &OpenCv::blobChanged, this, [](QImage img)
    {
        db().insert("image_blob", img);
    });

    connect(&opencv(), &OpenCv::smallRegionBlobImage, this, [](QImage img)
    {
        db().insert("image_small_blob_captured", img);
    });

    TaskScan* taskScan = new TaskScan(this);
    TaskUpdate* taskUpdate = new TaskUpdate(this);
    TaskTestScanUpdateCycle* tp = new TaskTestScanUpdateCycle(taskScan, taskUpdate, this);
    TaskCheckCamera* taskCheckCamera = new TaskCheckCamera(this);
    TaskTestAlgo* ta = new TaskTestAlgo(this);
    TaskPunch* taskPunch = new TaskPunch(this);
    TaskCheckPixelSize* taskCheckPixelSize = new TaskCheckPixelSize(this);

    qd() << "styles" << QQuickStyle::availableStyles();
    QQuickStyle::setStyle("Fusion");

    _qmlEngine.reset(new QQmlApplicationEngine());

    _qmlEngine->addImportPath(appDir() + "libs");

    qmlRegisterType<ImageItem>("ImageItem", 1, 0, "ImageItem");

    _qmlEngine->rootContext()->setContextProperty("applicationDirPath", QGuiApplication::applicationDirPath());
    _qmlEngine->rootContext()->setContextProperty("DataBus", &DataBus::instance());
    _qmlEngine->rootContext()->setContextProperty("Engine", this);
    //_qmlEngine->rootContext()->setContextProperty("Video3", _videoDriver3);
    _qmlEngine->rootContext()->setContextProperty("Video4", &video());
    _qmlEngine->rootContext()->setContextProperty("Serial", &Serial::instance());
    //_qmlEngine->rootContext()->setContextProperty("ImagesStorage", myImageProvider);
    _qmlEngine->rootContext()->setContextProperty("OpenCv", &opencv());

    _qmlEngine->rootContext()->setContextProperty("TaskScan", taskScan);
    _qmlEngine->rootContext()->setContextProperty("TaskUpdate", taskUpdate);
    _qmlEngine->rootContext()->setContextProperty("TaskTestScanUpdateCycle", tp);
    _qmlEngine->rootContext()->setContextProperty("TaskCheckCamera", taskCheckCamera);
    _qmlEngine->rootContext()->setContextProperty("TaskTestAlgo", ta);
    _qmlEngine->rootContext()->setContextProperty("TaskPunch", taskPunch);
    _qmlEngine->rootContext()->setContextProperty("TaskCheckPixelSize", taskCheckPixelSize);

    _qmlEngine->load(QUrl::fromLocalFile(appDir() + QString("gui/main.qml")));
}
