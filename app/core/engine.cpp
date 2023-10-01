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

Engine::Engine(QObject* parent)
    : QObject(parent)
    , _qmlEngine(nullptr)
    , _serial(new Serial)
    , _openCv(new OpenCv)
{
    _serial.reset(new Serial);

    DataBus& db = DataBus::instance();

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


    MyImageProvider* myImageProvider = new MyImageProvider;

    connect(_videoDriver4, &Video4::newImage, this, [this, myImageProvider](QImage img)
    {
        const QString mode = db().value("mode").toString();

        QString x = db().value("x_coord").toString();
        QString y = db().value("y_coord").toString();

        img.setText("x", x);
        img.setText("y", y);

        if (mode == "raw")
            myImageProvider->setImage(OpenCv::drawCross(img), "raw");

        if (mode == "circle")
            _openCv->searchCirclesLive(img);

        if (mode == "blob")
            _openCv->blobDetectorLive(img);
    });

    connect(_videoDriver4, &Video4::captured, this, [this, myImageProvider](QImage img)
    {
        int captureNumber = db().value("capture_number").toInt();
        QString x = db().value("x_coord").toString();
        QString y = db().value("y_coord").toString();
        img.setText("x", x);
        img.setText("y", y);

        myImageProvider->setImage(img, "raw captured");
        myImageProvider->setImage(_openCv->drawText(img.copy(), x + " " + y), QString("captured_%1").arg(captureNumber));

        _openCv->blobDetectorCaptured(img.copy());
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

    _qmlEngine.reset(new QQmlApplicationEngine());

    _qmlEngine->addImportPath(appDir() + "libs");
    _qmlEngine->addImageProvider("camera", myImageProvider);

    _qmlEngine->rootContext()->setContextProperty("DataBus", &DataBus::instance());
    _qmlEngine->rootContext()->setContextProperty("Engine", this);
    _qmlEngine->rootContext()->setContextProperty("Video3", _videoDriver3);
    _qmlEngine->rootContext()->setContextProperty("Video4", _videoDriver4);
    _qmlEngine->rootContext()->setContextProperty("Serial", _serial.data());
    _qmlEngine->rootContext()->setContextProperty("ImagesStorage", myImageProvider);
    _qmlEngine->rootContext()->setContextProperty("OpenCv", _openCv);

    _qmlEngine->load(QUrl::fromLocalFile(appDir() + QString("gui/main.qml")));
}
