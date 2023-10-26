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
#include "search_blobs.h"
#include "update_blobs.h"

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
    _videoDriver4 = new Video4();

    createQmlEngine();
}


QStringList Engine::camerasInfo()
{
    return _info;
}

QStringList Engine::removeDuplicatedBlobs(QStringList blobs)
{
    ScopedMeasure("remove duplicated blobs");

    QGraphicsScene scene;

    // Отправляю все блобы на сцену
    for (const QString& blob : blobs)
    {
//        QStringList coord = blob.split(" ");
//        double x = coord[0].toDouble();
//        double y = coord[1].toDouble();
        auto [x, y, dia] = blobToDouble(blob);

        QGraphicsEllipseItem* item = scene.addEllipse(-dia/2, -dia/2, dia, dia);
        item->setPos(x, y);
    }

    // если есть пересечение с кем то, то удалить его
    for (QGraphicsItem* item : qAsConst(scene).items())
    {
        if (!scene.collidingItems(item).isEmpty())
            delete item;
    }

    QStringList b;
    for (const QGraphicsItem* item : qAsConst(scene).items())
    {
        b.append(toReal3(item->x()) + " " + toReal3(item->y()));
    }

    return b;
}

Engine::~Engine()
{
    _qmlEngine.reset();
}

void Engine::createQmlEngine()
{
    MyImageProvider* myImageProvider = new MyImageProvider;

    connect(_videoDriver4, &Video4::newImage, this, [myImageProvider](QImage img)
    {
        const QString mode = db().value("mode").toString();

        const QString x = db().value("x_coord").toString();
        const QString y = db().value("y_coord").toString();

        img.setText("x", x);
        img.setText("y", y);

        if (mode == "raw")
            myImageProvider->setImage(OpenCv::drawCross(img), "raw");

        if (mode == "circle")
            opencv().searchCirclesLive(img);

        if (mode == "blob")
            opencv().blobDetectorLive(img);
    });

    connect(_videoDriver4, &Video4::capturedSmallRegion, this, [myImageProvider](QImage img)
    {
        myImageProvider->setImage(opencv().drawCross(img.copy()), "raw captured");
    });

    connect(_videoDriver4, &Video4::captured, this, [myImageProvider](QImage img)
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

    SearchBlobs* sb = new SearchBlobs(_videoDriver4, this);
    UpdateBlobs* ub = new UpdateBlobs(_videoDriver4, this);

    qd() << "styles" << QQuickStyle::availableStyles();
    QQuickStyle::setStyle("Fusion");

    _qmlEngine.reset(new QQmlApplicationEngine());

    _qmlEngine->addImportPath(appDir() + "libs");
    _qmlEngine->addImageProvider("camera", myImageProvider);

    _qmlEngine->rootContext()->setContextProperty("DataBus", &DataBus::instance());
    _qmlEngine->rootContext()->setContextProperty("Engine", this);
    _qmlEngine->rootContext()->setContextProperty("Video3", _videoDriver3);
    _qmlEngine->rootContext()->setContextProperty("Video4", _videoDriver4);
    _qmlEngine->rootContext()->setContextProperty("Serial", &Serial::instance());
    _qmlEngine->rootContext()->setContextProperty("ImagesStorage", myImageProvider);
    _qmlEngine->rootContext()->setContextProperty("OpenCv", &opencv());

    _qmlEngine->rootContext()->setContextProperty("SearchBlobs", sb);
    _qmlEngine->rootContext()->setContextProperty("UpdateBlobs", ub);

    _qmlEngine->load(QUrl::fromLocalFile(appDir() + QString("gui/main.qml")));
}
