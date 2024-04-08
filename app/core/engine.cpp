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
#include <QTimer>
#include <QDir>

#include "log/logger.h"
#include "data_bus.h"
#include "task_scan.h"
#include "task_update.h"
#include "task_test_scan_update_cycle.h"
#include "task_test_algo.h"
#include "task_check_camera.h"
#include "task_punch.h"
#include "task_find_pixel_size.h"
//#include "task_best_path.h"
#include "task_find_blob.h"

#include "openCv.h"


#include "placeholder_quick_item.h"
#include "qml_graphics_view.h"
#include "my_image_provider.h"

#include "scene.h"
#include "settings.h"

#include "ImageItem.h"
#include "quick_scene.h"
#include "graphics_view.h"

#include "file_system_watcher.h"
#include "widget_anchor.h"
#include "mainwindow3.h"

#include <QQuickWidget>
#include <QMainWindow>


EnhancedQmlApplicationEngine::EnhancedQmlApplicationEngine(QObject *parent)
    : QQmlApplicationEngine(parent)
{

}

void EnhancedQmlApplicationEngine::clearCache()
{
    trimComponentCache();
    clearComponentCache();
    trimComponentCache();

    qd() << "cache cleared";
}


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
    //splash.show();

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
//     });engin
//     auto guard = qScopeGuard([=]() { disconnect(connection); });
//     video().start();
//     video().capture();
//     waitForSignal(&video(), &Video4::captured, 2000);
// }

// void Engine::corr()
// {
//     opencv().corr();
// }

// void Engine::capture()
// {
//     auto connection = connect(&video(), &Video4::captured, &scene(), &Scene::setImage);
//     auto guard = qScopeGuard([=]() { disconnect(connection); });
//     video().start();
//     video().capture();
//     waitForSignal(&video(), &Video4::captured, 2000);
// }

Engine::~Engine()
{
    //_qmlEngine.reset();
}

#include <QVBoxLayout>

void Engine::createQmlEngine()
{
    connect(&video(), &Video4::rawImage, this, [](QImage img)
    {
        const QString livePreviewMode = db().value("live_preview_mode").toString();

        if (livePreviewMode == "raw")
            db().insert("live_preview_image_raw", img.copy());

        if (livePreviewMode == "circle")
            opencv().searchCirclesLive(img.copy());

        if (livePreviewMode == "blob" || livePreviewMode == "adapt_threshold")
        {
            opencv().blobDetectorLive(img.copy(), db().value("blob_live_options").toMap());

            //qd() << "blob live options "  << db().value("blob_live_options").toMap();
        }
    });

    connect(&video(), &Video4::capturedSmallRegion, this, [](QImage img)
    {
        db().insert("live_preview_image_raw_captured", img.copy());
    });

    connect(&opencv(), &OpenCv::circleChanged, this, [](QImage img)
    {
        db().insert("live_preview_image_circle", img);
    });

    connect(&opencv(), &OpenCv::smallRegionBlobImage, this, [](QImage img)
    {
        db().insert("live_preview_image_small_blob_captured", img);
    });

    TaskScan* taskScan = new TaskScan(this);

    TaskUpdate* taskUpdate = new TaskUpdate(this);
    TaskTestScanUpdateCycle* tp = new TaskTestScanUpdateCycle(taskScan, taskUpdate, this);
    TaskCheckCamera* taskCheckCamera = new TaskCheckCamera(this);
    TaskTestAlgo* ta = new TaskTestAlgo(this);
    TaskPunch* taskPunch = new TaskPunch(this);
    TaskFindPixelSize* taskFindPixelSize = new TaskFindPixelSize(this);
    //TaskBestPath* taskBestPath = new TaskBestPath(this);
    TaskFindBlob* taskFindBlob = new TaskFindBlob(this);

    FileSystemWatcher* filesystemwatcher = new FileSystemWatcher(this);

    qd() << "";
    qd() << "styles" << QQuickStyle::availableStyles();
    QQuickStyle::setStyle("Fusion");

    /// 1. Окно
    _mw.reset(new MainWindow3);


    /// 2. Виджеты в таком порядке
    _quickWidget = new QQuickWidget(_mw->centralWidget());

    GraphicsView* gw = new GraphicsView(_mw->centralWidget());

    _quickWidget2 = new QQuickWidget(_mw->centralWidget());


    // QVBoxLayout* verticalLayout = new QVBoxLayout(_mw->centralWidget());
    // verticalLayout->addWidget(_quickWidget);

    _mw->centralWidget()->layout()->addWidget(_quickWidget);


    //_qmlEngine.reset(new EnhancedQmlApplicationEngine());
    _qmlEngine = _quickWidget->engine();

    _qmlEngine->addImportPath(appDir() + "libs");

    qmlRegisterType<ImageItem>("ImageItem", 1, 0, "ImageItem");
    qmlRegisterType<QuickScene>("QuickScene", 1, 0, "QuickScene");

    qmlRegisterType<QmlGraphicsView>("QmlGraphicsView", 1, 0, "QmlGraphicsView");

    // Неудачный эксперимент
    //qmlRegisterType<PlaceholderQuickItem>("PlaceholderQuickItem", 1, 0, "PlaceholderQuickItem");

    _qmlEngine->rootContext()->setContextProperty("applicationDirPath", QGuiApplication::applicationDirPath());
    _qmlEngine->rootContext()->setContextProperty("DataBus", &DataBus::instance());
    _qmlEngine->rootContext()->setContextProperty("Engine", this);
    //_qmlEngine->rootContext()->setContextProperty("Video3", _videoDriver3);
    _qmlEngine->rootContext()->setContextProperty("Video4", &video());
    _qmlEngine->rootContext()->setContextProperty("Serial", &Serial::instance());
    //_qmlEngine->rootContext()->setContextProperty("ImagesStorage", myImageProvider);
    _qmlEngine->rootContext()->setContextProperty("OpenCv", &opencv());
    _qmlEngine->rootContext()->setContextProperty("Settings", &Settings::instance());
    _qmlEngine->rootContext()->setContextProperty("Logger", &Logger::instance());

    _qmlEngine->rootContext()->setContextProperty("TaskScan", taskScan);
    _qmlEngine->rootContext()->setContextProperty("TaskUpdate", taskUpdate);
    _qmlEngine->rootContext()->setContextProperty("TaskTestScanUpdateCyoverlayItemcle", tp);
    _qmlEngine->rootContext()->setContextProperty("TaskCheckCamera", taskCheckCamera);
    _qmlEngine->rootContext()->setContextProperty("TaskTestAlgo", ta);
    _qmlEngine->rootContext()->setContextProperty("TaskPunch", taskPunch);
    _qmlEngine->rootContext()->setContextProperty("TaskFindPixelSize", taskFindPixelSize);
    //_qmlEngine->rootContext()->setContextProperty("TaskBestPath", taskBestPath);
    _qmlEngine->rootContext()->setContextProperty("TaskFindBlob", taskFindBlob);


    //_qmlEngine->rootContext()->setContextProperty("QmlEngine", _qmlEngine.data());

    _qmlEngine->rootContext()->setContextProperty("GraphicsScene", &scene());

    _qmlEngine->rootContext()->setContextProperty("FileSystemWatcher", filesystemwatcher);
    //_qmlEngine->load(QUrl::fromLocalFile(appDir() + QString("gui/main.qml")));
    _quickWidget->setSource(QUrl::fromLocalFile(appDir() + QString("gui/OperatorUser.qml")));



    /// 3. Важные настройки
    //QQuickWindow::setDefaultAlphaBuffer(true); // Вроде бы должна быть полезна, но толку от нее не нашел.
    //_quickWidget2->setWindowFlags(Qt::SplashScreen);
    _quickWidget2->setAttribute(Qt::WA_AlwaysStackOnTop); // Ключевой параметр
    _quickWidget2->setAttribute(Qt::WA_TranslucentBackground);
    _quickWidget2->setClearColor(Qt::transparent);
    _quickWidget2->setResizeMode(QQuickWidget::SizeRootObjectToView);
    //_quickWidget2->setAttribute(Qt::WA_TransparentForMouseEvents); // Работает, но обошелся без этого
    //_quickWidget2->setSource(QUrl::fromLocalFile(appDir() + QString("gui/overlay.qml")));
    //_quickWidget2->show();
    //_quickWidget2->move(60,60);

    QQuickItem* graphicsViewPlaceholder = _quickWidget->rootObject()->findChild<QQuickItem*>("overlayItem");
    new WidgetAnchor(gw, graphicsViewPlaceholder);
    //gw->resize(300, 300);
    // gw->setEnabled(true);
    // gw->setVisible(true);
    //gw->move(50,50);
     gw->setScene(&scene());
    // //gw->show();
     _mw->show();

    // _quickWidget->resize(_mw->size());
     // _quickWidget->setEnabled(true);
     // _quickWidget->setVisible(true);
     // _quickWidget->show();

}

void Engine::reload()
{
    //_qmlEngine.reset(new EnhancedQmlApplicationEngine());
    //_qmlEngine->rootContext()->setContextProperty("applicationDirPath", QGuiApplication::applicationDirPath());
    //_qmlEngine->load(QUrl::fromLocalFile(appDir() + QString("gui/main.qml")));
    _quickWidget->setSource(QUrl::fromLocalFile(appDir() + QString("gui/main.qml")));
}

QStringList Engine::filesInDirectory(QString dir) const
{
    return ::filesInDirectory(dir, QStringList{"*.ini"});
}

QVariantMap Engine::readFile(QString path)
{
    return openIniFile(path);
}

void Engine::saveFile(QString path, QVariantMap options)
{
    saveIniFile(path, options);
}

QStringList Engine::filesInSceneDirectory() const
{
    return filesInDirectory(QString("find_blob_scenes"));
}

QVariantMap Engine::readSceneFile(QString filename) const
{
    return openIniFile(QString("find_blob_scenes/") + filename);
}

void Engine::saveSceneFile(QString filename, QVariantMap options)
{
    saveIniFile(QString("find_blob_scenes/") + filename, options);
}
