#include "engine.h"

#include "serial.h"
#include "version.h" // автогенерированный файл qmake из version.h.in
#include "utils.h"

#include <QCoreApplication>
#include <QQmlContext>
#include <QApplication>
#include <QQmlApplicationEngine>
#include <QProcess>

#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>

namespace {

void drawCircles(const cv::Mat& image, const std::vector<cv::Vec3f>& circles)
{
    for (size_t i = 0; i < circles.size(); i++ )
    {
        cv::Vec3i c = circles[i];
        cv::Point center = cv::Point(c[0], c[1]);
        // circle center
        cv::circle(image, center, 1, cv::Scalar(255,0,255), 4, cv::LINE_AA);
        // circle outline
        int radius = c[2];
        cv::circle(image, center, radius, cv::Scalar(255,0,0), 4, cv::LINE_AA);
    }
}


}

Engine::Engine(QObject* parent)
    : QObject(parent)
    , _qmlEngine(nullptr)
{
    _serial.reset(new Serial);

    createQmlEngine();
}

Engine::~Engine()
{
    _qmlEngine.reset();
}

void Engine::createQmlEngine()
{
    _qmlEngine.reset(new QQmlApplicationEngine());

    _qmlEngine->addImportPath(appDir() + "libs");

    _qmlEngine->rootContext()->setContextProperty("Engine", this);
    _qmlEngine->rootContext()->setContextProperty("Serial", _serial.data());
    _qmlEngine->load(QUrl::fromLocalFile(appDir() + QString("gui/main.qml")));


    cv::namedWindow("grey", cv::WINDOW_NORMAL | cv::WINDOW_GUI_EXPANDED);
    cv::namedWindow("blur", cv::WINDOW_NORMAL | cv::WINDOW_GUI_EXPANDED);
    cv::namedWindow("main", cv::WINDOW_NORMAL | cv::WINDOW_GUI_EXPANDED);
}

void Engine::update()
{
    QProcess process;

    process.start("/bin/sh", {"-c", "rm /dev/shm/img.png"});
    process.waitForFinished();

    process.start("/bin/sh", {"-c", _photoCommand});
    process.waitForFinished();
    qd() << process.readAllStandardOutput().trimmed();
    qd() << process.readAllStandardError().trimmed();

    cv::Mat image = cv::imread("/dev/shm/img.png");

    cv::Mat grey;
    {
        ScopedMeasure m("color");
        cv::cvtColor(image, grey, cv::COLOR_BGR2GRAY);
    }


    cv::imshow("grey", grey);
    //cv::resizeWindow("grey", cv::Size(grey.size().width / 4, grey.size().height / 4));

    cv::Mat blur;
    medianBlur(grey, blur, 3);


    cv::imshow("blur", blur);
    //cv::resizeWindow("blur", cv::Size(blur.size().width / 4, blur.size().height / 4));

    std::vector<cv::Vec3f> circles;
    {
        ScopedMeasure m("circles");
        cv::HoughCircles(blur, circles, cv::HOUGH_GRADIENT, 1.2, 70, 168, 29, 80, 110);
    }

    drawCircles(image, circles);


    cv::imshow("main", image);
    cv::resizeWindow("main", cv::Size(image.size().width / 4, image.size().height / 4));
}

void Engine::setPhotoCommand(QString cmd)
{
    _photoCommand = cmd;
}


