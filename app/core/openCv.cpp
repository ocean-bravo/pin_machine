#include "openCv.h"
#include "utils.h"

#include "data_bus.h"

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
        cv::circle(image, center, 1, cv::Scalar(255,0,255), 3, cv::LINE_AA);
        // circle outline
        int radius = c[2];
        cv::circle(image, center, radius, cv::Scalar(255,0,0), 1, cv::LINE_AA);
    }
}

cv::Mat qimage_to_mat_cpy( QImage img, int format)
{
    //img = img.convertToFormat(QImage::Format_RGB888, Qt::ColorOnly).rgbSwapped();
    img = img.convertToFormat(QImage::Format_BGR888, Qt::ColorOnly);
    return cv::Mat(img.height(), img.width(), format, img.bits(), img.bytesPerLine()).clone();
}

QImage mat_to_qimage_ref(cv::Mat &mat, QImage::Format format)
{
    return QImage(mat.data, mat.cols, mat.rows, mat.step, format);
}

}


OpenCv::OpenCv()
    : _impl(new OpenCvPrivate)
    , _thread(new QThread)
{
    connect(_impl, &OpenCvPrivate::imageChanged,   this, &OpenCv::imageChanged, Qt::QueuedConnection);
    //    connect(this, &Logger::common, _impl, &LoggerPrivate::common, Qt::QueuedConnection);

    //    connect(_impl, &LoggerPrivate::logToFileChanged, this, &Logger::logToFileChanged, Qt::QueuedConnection);
    //    connect(_impl, &LoggerPrivate::inited,           this, &Logger::inited, Qt::QueuedConnection);

    connect(_thread.data(), &QThread::finished, _impl, &QObject::deleteLater);
    connect(_thread.data(), &QThread::started,  _impl, &OpenCvPrivate::init, Qt::QueuedConnection);

    _impl->moveToThread(_thread.data());
    _thread->start();
}

OpenCv::~OpenCv()
{
    _thread->quit();
    _thread->wait(1000);
}

void OpenCv::searchCircles(QImage img)
{
    QMetaObject::invokeMethod(_impl, "searchCircles", Qt::QueuedConnection, Q_ARG(QImage, img));
}


OpenCvPrivate::OpenCvPrivate()
{
    db().insert("dp", 1.2);
    db().insert("minDist", 70);
    db().insert("param1", 168);
    db().insert("param2", 29);
    db().insert("minRadius", 80);
    db().insert("maxRadius", 110);
}

OpenCvPrivate::~OpenCvPrivate()
{

}

void OpenCvPrivate::init()
{

}

void OpenCvPrivate::searchCircles(QImage img)
{
    if (_done)
    {
        _done = false;
    }
    else
    {
        return;
    }

    try
    {
        cv::Mat image = qimage_to_mat_cpy(img,  CV_8UC3);
        cv::Mat grey;
        {
            ScopedMeasure m("color");
            cv::cvtColor(image, grey, cv::COLOR_BGR2GRAY);
        }

        //cv::imshow("grey", grey);
        //cv::resizeWindow("grey", cv::Size(grey.size().width / 4, grey.size().height / 4));

        cv::Mat blur;
        cv::medianBlur(grey, blur, 3);

        //cv::imshow("blur", blur);
        //cv::resizeWindow("blur", cv::Size(blur.size().width / 4, blur.size().height / 4));

        std::vector<cv::Vec3f> circles;
        {
            ScopedMeasure m("circles");

            double dp = db().value("dp").toDouble();
            double minDist = db().value("minDist").toDouble();
            double param1 = db().value("param1").toDouble();
            double param2 = db().value("param2").toDouble();
            int minRadius = db().value("minRadius").toInt();
            int maxRadius = db().value("maxRadius").toInt();


            cv::HoughCircles(blur, circles, cv::HOUGH_GRADIENT, dp, minDist, param1, param2, minRadius, maxRadius);
            //cv::HoughCircles(blur, circles, cv::HOUGH_GRADIENT, dp, 70, 168, 29, 80, 110);
        }

        drawCircles(image, circles);

        //cv::imshow("main", image);
        //cv::resizeWindow("main", cv::Size(image.size().width / 4, image.size().height / 4));

        emit imageChanged(mat_to_qimage_ref(image, QImage::Format_BGR888));
    }
    catch (...)
    {
        qd() << "упало";
    }

    _done = true;
}
