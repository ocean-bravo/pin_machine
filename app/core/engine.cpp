#include "engine.h"

#include "serial.h"
#include "version.h" // автогенерированный файл qmake из version.h.in
#include "utils.h"
#include "utils2.h"

#include <QCoreApplication>
#include <QQmlContext>
#include <QApplication>
#include <QQmlApplicationEngine>
#include <QProcess>
#include <QBuffer>
#include <QGraphicsScene>
#include <QGraphicsItem>

#include <QQuickStyle>

#include "data_bus.h"
#include "task_scan.h"
#include "task_update.h"
#include "task_test.h"
#include "task_test_algo.h"
#include "task_punch.h"

#include "openCv.h"

#include "my_image_provider.h"

Engine::Engine(QObject* parent)
    : QObject(parent)
    , _qmlEngine(nullptr)
{
    Serial::instance();

    DataBus::instance();
    OpenCv::instance();

    _videoDriver3 = new Video3();
    Video4::instance();

    createQmlEngine();
}


QStringList Engine::camerasInfo()
{
    return _info;
}

Engine::~Engine()
{
    _qmlEngine.reset();
}

void Engine::createQmlEngine()
{
    MyImageProvider* myImageProvider = new MyImageProvider;

    connect(&video(), &Video4::newImage, this, [myImageProvider](QImage img)
    {
        const QString mode = db().value("mode").toString();

        if (mode == "raw")
            myImageProvider->setImage(OpenCv::drawCross(img), "raw");

        if (mode == "circle")
            opencv().searchCirclesLive(img);

        if (mode == "blob")
            opencv().blobDetectorLive(img);
    });

    connect(&video(), &Video4::capturedSmallRegion, this, [myImageProvider](QImage img)
    {
        myImageProvider->setImage(opencv().drawCross(img.copy()), "raw captured");
    });

    connect(&video(), &Video4::captured, this, [myImageProvider](QImage img)
    {
        int captureNumber = db().value("capture_number").toInt();
        const QString x = db().value("x_coord").toString();
        const QString y = db().value("y_coord").toString();

        qd() << "captured " << captureNumber << x << y;

        img.setText("x", x);
        img.setText("y", y);

        myImageProvider->setImage(img, "raw captured");
        //myImageProvider->setImage(_openCv->drawText(img.copy(), x + " " + y), QString("captured_%1").arg(captureNumber));

        opencv().blobDetectorCaptured(img.copy());
    });

    connect(myImageProvider, &MyImageProvider::imageChanged, this, &Engine::imageChanged);

    connect(&opencv(), &OpenCv::circleChanged, this, [myImageProvider](QImage img)
    {
        myImageProvider->setImage(img, "circle");
    });

    connect(&opencv(), &OpenCv::blobChanged, this, [myImageProvider](QImage img)
    {
        myImageProvider->setImage(img, "blob");
    });

    connect(&opencv(), &OpenCv::smallRegionBlobImage, this, [myImageProvider](QImage img)
    {
        myImageProvider->setImage(img, "small_blob_captured");
    });

    TaskScan* sb = new TaskScan(this);
    TaskUpdate* ub = new TaskUpdate(this);
    TaskTest* tp = new TaskTest(sb, ub, this);
    TaskPunch* pu = new TaskPunch(this);
    TaskTestAlgo* ta = new TaskTestAlgo(this);

    qd() << "styles" << QQuickStyle::availableStyles();
    QQuickStyle::setStyle("Fusion");

    _qmlEngine.reset(new QQmlApplicationEngine());

    _qmlEngine->addImportPath(appDir() + "libs");
    _qmlEngine->addImageProvider("camera", myImageProvider);

    _qmlEngine->rootContext()->setContextProperty("DataBus", &DataBus::instance());
    _qmlEngine->rootContext()->setContextProperty("Engine", this);
    _qmlEngine->rootContext()->setContextProperty("Video3", _videoDriver3);
    _qmlEngine->rootContext()->setContextProperty("Video4", &video());
    _qmlEngine->rootContext()->setContextProperty("Serial", &Serial::instance());
    _qmlEngine->rootContext()->setContextProperty("ImagesStorage", myImageProvider);
    _qmlEngine->rootContext()->setContextProperty("OpenCv", &opencv());

    _qmlEngine->rootContext()->setContextProperty("SearchBlobs", sb);
    _qmlEngine->rootContext()->setContextProperty("UpdateBlobs", ub);
    _qmlEngine->rootContext()->setContextProperty("TestProgram", tp);
    _qmlEngine->rootContext()->setContextProperty("Punch", pu);
    _qmlEngine->rootContext()->setContextProperty("TaskTestAlgo", ta);

    _qmlEngine->load(QUrl::fromLocalFile(appDir() + QString("gui/main.qml")));
}
