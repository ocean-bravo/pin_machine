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

#include <QJsonObject>

#include <QQuickStyle>
#include <QScopeGuard>
#include <QTimer>
#include <QDir>
#include <QScreen>

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
#include "mainwindow.h"
#include "mainwindow3.h"

#include <QQuickWidget>


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


    qmlRegisterType<ImageItem>("ImageItem", 1, 0, "ImageItem");
    qmlRegisterType<QuickScene>("QuickScene", 1, 0, "QuickScene");
    qmlRegisterType<QmlGraphicsView>("QmlGraphicsView", 1, 0, "QmlGraphicsView");
    // Неудачный эксперимент
    //qmlRegisterType<PlaceholderQuickItem>("PlaceholderQuickItem", 1, 0, "PlaceholderQuickItem");

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


    _sceneView.reset(new MainWindow);

    QRect screenrect = QGuiApplication::primaryScreen()->geometry();
    _sceneView->resize(1920 / 2, screenrect.height());
    _sceneView->move(1920/2, screenrect.top());
    _sceneView->show();

    /// 1. Окно
    _mw.reset(new MainWindow3);
    // _mw->setAttribute(Qt::WA_OpaquePaintEvent);
    // _mw->setAttribute(Qt::WA_NoSystemBackground);
    // _mw->setAttribute(Qt::WA_TranslucentBackground);

    /// 2. Виджеты в таком порядке добавляются на центральный виджет
    _quickWidget = new QQuickWidget(_mw->centralWidget());
    _gw = new GraphicsView(_mw->centralWidget());
    _quickWidget2 = new QQuickWidget(_mw->centralWidget());

    /// 3. Чтобы растянуло виджет самый нижний
    _mw->centralWidget()->layout()->addWidget(_quickWidget);

    /// 4. Чтобы растянуло виджет самый верхний по размеру самого нижнего
    /// Костылище. Нужно держать верхний виджет по размеру нижнего. И layout не подходит. По хорошему отнаследоваться бы от QQuickWidget.
    QTimer* resizeTimer = new QTimer(this);
    connect(resizeTimer, &QTimer::timeout, this, [this]() { _quickWidget2->resize(_quickWidget->size()); });
    resizeTimer->start(100);


    QQmlEngine* qmlEngine = _quickWidget->engine();
    qmlEngine->addImportPath(appDir() + "libs");

    qmlEngine->rootContext()->setContextProperty("applicationDirPath", QGuiApplication::applicationDirPath());
    qmlEngine->rootContext()->setContextProperty("DataBus", &DataBus::instance());
    qmlEngine->rootContext()->setContextProperty("Engine", this);
    //_qmlEngine->rootContext()->setContextProperty("Video3", _videoDriver3);
    qmlEngine->rootContext()->setContextProperty("Video4", &video());
    qmlEngine->rootContext()->setContextProperty("Serial", &Serial::instance());
    //_qmlEngine->rootContext()->setContextProperty("ImagesStorage", myImageProvider);
    qmlEngine->rootContext()->setContextProperty("OpenCv", &opencv());
    qmlEngine->rootContext()->setContextProperty("Settings", &Settings::instance());
    qmlEngine->rootContext()->setContextProperty("Logger", &Logger::instance());

    qmlEngine->rootContext()->setContextProperty("TaskScan", taskScan);
    qmlEngine->rootContext()->setContextProperty("TaskUpdate", taskUpdate);
    qmlEngine->rootContext()->setContextProperty("TaskTestScanUpdateCyoverlayItemcle", tp);
    qmlEngine->rootContext()->setContextProperty("TaskCheckCamera", taskCheckCamera);
    qmlEngine->rootContext()->setContextProperty("TaskTestAlgo", ta);
    qmlEngine->rootContext()->setContextProperty("TaskPunch", taskPunch);
    qmlEngine->rootContext()->setContextProperty("TaskFindPixelSize", taskFindPixelSize);
    //_qmlEngine->rootContext()->setContextProperty("TaskBestPath", taskBestPath);
    qmlEngine->rootContext()->setContextProperty("TaskFindBlob", taskFindBlob);
    qmlEngine->rootContext()->setContextProperty("MainWindow", _mw.data());
    qmlEngine->rootContext()->setContextProperty("QmlEngine", qmlEngine);
    qmlEngine->rootContext()->setContextProperty("GraphicsScene", &scene());
    qmlEngine->rootContext()->setContextProperty("FileSystemWatcher", filesystemwatcher);
    _quickWidget->setSource(QUrl::fromLocalFile(appDir() + QString("gui/main.qml")));


    QQmlEngine* qmlEngine2 = _quickWidget2->engine();
    qmlEngine2->rootContext()->setContextProperty("DataBus", &DataBus::instance());
    qmlEngine2->rootContext()->setContextProperty("MainWindow", _mw.data());
    qmlEngine2->rootContext()->setContextProperty("QuickWidget2", _quickWidget2);
    qmlEngine2->rootContext()->setContextProperty("QuickWidget", _quickWidget);
    qmlEngine2->rootContext()->setContextProperty("Engine", this);
    _quickWidget2->setSource(QUrl::fromLocalFile(appDir() + QString("gui/main_overlay.qml")));


    /// 4. Важные настройки
    _quickWidget2->setAttribute(Qt::WA_AlwaysStackOnTop); // Ключевой параметр
    _quickWidget2->setAttribute(Qt::WA_TranslucentBackground);
    _quickWidget2->setClearColor(Qt::transparent);
    _quickWidget2->setResizeMode(QQuickWidget::SizeRootObjectToView);
    _quickWidget2->setAttribute(Qt::WA_TransparentForMouseEvents);


    /// 5. Важно
    QQuickItem* graphicsViewPlaceholder = _quickWidget->rootObject()->findChild<QQuickItem*>("placeholderForGraphicsView");
    _widgetAnchor = new WidgetAnchor(_gw, graphicsViewPlaceholder);


    _gw->setScene(&scene());
    _mw->showMaximized();
}


void Engine::reinit()
{

}

void Engine::reload()
{
    //_qmlEngine.reset(new EnhancedQmlApplicationEngine());
    //_qmlEngine->rootContext()->setContextProperty("applicationDirPath", QGuiApplication::applicationDirPath());
    //_qmlEngine->load(QUrl::fromLocalFile(appDir() + QString("gui/main.qml")));

    _quickWidget->rootObject()->deleteLater();

    connect(_quickWidget->rootObject(), &QObject::destroyed, this, [=]
    {
        _quickWidget->engine()->trimComponentCache();
        _quickWidget->engine()->clearComponentCache();
        _quickWidget->engine()->trimComponentCache();
        _quickWidget->setSource(QUrl::fromLocalFile(appDir() + QString("gui/main.qml")));

        QQuickItem* graphicsViewPlaceholder = _quickWidget->rootObject()->findChild<QQuickItem*>("placeholderForGraphicsView");
        _widgetAnchor->deleteLater();

        // Так надо
        connect(_widgetAnchor, &QObject::destroyed, this, [=]
        {
            _widgetAnchor = new WidgetAnchor(_gw, graphicsViewPlaceholder);
        });
    });
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

void Engine::setOverlayWidgetTransparent(bool state)
{
    _quickWidget2->setAttribute(Qt::WA_TransparentForMouseEvents, state); // Работает, но обошелся без этого
}
