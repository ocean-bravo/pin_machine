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
    //_videoDriver3->reloadDevices();
    //_videoDriver3->changeCamera(0, 0);

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
    connect(_videoDriver3, &Video3::newImage, this, [this, myImageProvider](QImage img, QString str, QByteArray imgPpm)
    {
        //_image = img;
        //qd() << "new image";
        myImageProvider->setImage(img, "raw");
        emit imageCaptured();
        _openCv->searchCircles(img);
    });


    connect(_openCv, &OpenCv::imageChanged, this, [this, myImageProvider](QImage img)
    {
        myImageProvider->setImage(img, "main");
    });

    qd() << "styles" << QQuickStyle::availableStyles();
    QQuickStyle::setStyle("Universal");

    _qmlEngine->addImageProvider("camera", myImageProvider);

    _qmlEngine->rootContext()->setContextProperty("DataBus", &DataBus::instance());
    _qmlEngine->rootContext()->setContextProperty("Engine", this);
    _qmlEngine->rootContext()->setContextProperty("Video", _videoDriver3);
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

