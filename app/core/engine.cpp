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
            _openCv->searchCircles(img);

        if (mode == "blob")
            _openCv->blobDetector(img);
    });

    connect(_videoDriver4, &Video4::captured, this, [this, myImageProvider](QImage img)
    {
        myImageProvider->setImage(img, "raw captured");

        // Где то взять номер
        int captureNumber = db().value("capture_number").toInt();
        myImageProvider->setImage(_openCv->drawText(img, "Hello"), QString("captured_%1").arg(captureNumber));

        _openCv->addToDetectBlobQueue(img);

    });

    connect(myImageProvider, &MyImageProvider::imageChanged, this, &Engine::imageChanged);

    connect(_openCv, &OpenCv::imageChanged, this, [this, myImageProvider](QImage img)
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



//void Engine::searchCircles(const QImage & img)
//{
//    //cv::Mat image = cv::imread("/dev/shm/cap.bmp");

//    cv::Mat image = qimage_to_mat_cpy(img, CV_32F);
//    cv::Mat grey;
//    {
//        ScopedMeasure m("color");
//        cv::cvtColor(image, grey, cv::COLOR_BGR2GRAY);
//    }


//    //cv::imshow("grey", grey);
//    //cv::resizeWindow("grey", cv::Size(grey.size().width / 4, grey.size().height / 4));

//    cv::Mat blur;
//    medianBlur(grey, blur, 3);


//    //cv::imshow("blur", blur);
//    //cv::resizeWindow("blur", cv::Size(blur.size().width / 4, blur.size().height / 4));

//    std::vector<cv::Vec3f> circles;
//    {
//        ScopedMeasure m("circles");
//        cv::HoughCircles(blur, circles, cv::HOUGH_GRADIENT, 1.2, 70, 168, 29, 80, 110);
//    }

//    drawCircles(image, circles);


//    cv::imshow("main", image);
//    cv::resizeWindow("main", cv::Size(image.size().width / 4, image.size().height / 4));
//}

void Engine::setPhotoCommand(QString cmd)
{
    _photoCommand = cmd;
}

void Engine::update()
{
     //QMetaObject::invokeMethod(_videoDriver3, "update", Qt::QueuedConnection);


//    QString image;
//    {
//        ScopedMeasure mes("convert image");
//        //QByteArray byteArray;
//        //QBuffer buffer(&byteArray);
//        //buffer.open(QIODevice::WriteOnly);
//        //_image.save(&buffer,"PNG");
//        //save image data in string
//        //image = "data:image/png;base64,";
//        image = "data:image/x-portable-pixmap;base64,";
//        image.append(QString::fromLatin1(_imgPpm.toBase64().data()));
//    }
//    return image;
}

