#include "engine.h"

#include "serial.h"
#include "version.h" // автогенерированный файл qmake из version.h.in
#include "utils.h"

#include <QCoreApplication>
#include <QQmlContext>
#include <QApplication>
#include <QQmlApplicationEngine>
#include <QProcess>
#include <QBuffer>

#include <QQuickStyle>

#include "data_bus.h"

#include "openCv.h"

namespace {

}

Engine::Engine(QObject* parent)
    : QObject(parent)
    , _qmlEngine(nullptr)
    , _openCv(new OpenCv)
{
    _serial.reset(new Serial);

    DataBus& db = DataBus::instance();

    //_videoDriver.init("/dev/video0", 15, 640, 480, "YUYV"); // MJPG

    _videoDriver3 = new Video3();
    _videoDriver4 = new Video4();

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
    _qmlEngine.reset(new QQmlApplicationEngine());
    _qmlEngine->addImportPath(appDir() + "libs");

    MyImageProvider*    myImageProvider = new MyImageProvider;
    connect(_videoDriver4, &Video4::newImage, this, [this, myImageProvider](QImage img)
    {
        myImageProvider->setImage(img, "raw");

        QString mode = db().value("mode").toString();

        if (mode == "circle")
            _openCv->searchCircles(img.copy());

        if (mode == "blob")
            _openCv->blobDetector(img.copy());

    });

    connect(_videoDriver4, &Video4::captured, this, [this, myImageProvider](QImage img)
    {
        myImageProvider->setImage(img, "raw captured");

        int captureNumber = db().value("capture_number").toInt();
        QString x = db().value("x_coord").toString();
        QString y = db().value("y_coord").toString();

        myImageProvider->setImage(_openCv->drawText(img.copy(), x + " " + y), QString("captured_%1").arg(captureNumber));

        _openCv->addToDetectBlobQueue(img);

    });

    connect(myImageProvider, &MyImageProvider::imageChanged, this, &Engine::imageChanged);

    connect(_openCv, &OpenCv::circleChanged, this, [this, myImageProvider](QImage img)
    {
        myImageProvider->setImage(img, "circle");
    });


    connect(_openCv, &OpenCv::blobChanged, this, [this, myImageProvider](QImage img)
    {
        myImageProvider->setImage(img, "blob");
    });

    qd() << "styles" << QQuickStyle::availableStyles();
    QQuickStyle::setStyle("Fusion");

    _qmlEngine->addImageProvider("camera", myImageProvider);

    _qmlEngine->rootContext()->setContextProperty("DataBus", &DataBus::instance());
    _qmlEngine->rootContext()->setContextProperty("Engine", this);
    _qmlEngine->rootContext()->setContextProperty("Video3", _videoDriver3);
    _qmlEngine->rootContext()->setContextProperty("Video4", _videoDriver4);
    _qmlEngine->rootContext()->setContextProperty("Serial", _serial.data());
    _qmlEngine->load(QUrl::fromLocalFile(appDir() + QString("gui/main.qml")));
}
