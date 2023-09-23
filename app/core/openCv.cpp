#include "openCv.h"
#include "utils.h"

#include "data_bus.h"

#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>


namespace {

//In the header file add:

class Color
{
    public:
    static cv::Scalar ColorBlue;
    static cv::Scalar ColorRed;
    static cv::Scalar ColorGreen;
    static cv::Scalar ColorWhite;
    static cv::Scalar ColorBlack;
};

//In the cpp file add:

cv::Scalar Color::ColorBlue(0, 0, 255, 255); //rgb 0 0 255
cv::Scalar Color::ColorRed(255, 0, 0, 255); // rgb 255 0 0
cv::Scalar Color::ColorGreen(0, 255, 0, 255);
cv::Scalar Color::ColorWhite(255, 255, 255);
cv::Scalar Color::ColorBlack(0, 0, 0, 0);

void drawCircles(const cv::Mat& image, const std::vector<cv::Vec3f>& circles)
{
    for (const cv::Vec3f& c : circles)
    {
        const cv::Point center = cv::Point(c[0], c[1]);
        cv::circle(image, center, 1, Color::ColorBlue, 3, cv::LINE_AA);     // circle center
        cv::circle(image, center, c[2], Color::ColorGreen, 1, cv::LINE_AA); // circle outline
    }
}

void drawKeyPoints(const cv::Mat& image, const std::vector<cv::KeyPoint>& kps)
{
    for (const cv::KeyPoint & kp : kps)
    {
        cv::circle(image, kp.pt, 1, Color::ColorBlue, 3, cv::LINE_AA);          // circle center
        cv::circle(image, kp.pt, kp.size/2, Color::ColorGreen, 1, cv::LINE_AA); // circle outline
    }
}

cv::Mat qimage_to_mat_cpy( QImage img, int format)
{
    img = img.convertToFormat(QImage::Format_RGB888, Qt::ColorOnly).rgbSwapped();
    //img = img.convertToFormat(QImage::Format_BGR888, Qt::ColorOnly);
    return cv::Mat(img.height(), img.width(), format, img.bits(), img.bytesPerLine()).clone();
}

QImage mat_to_qimage_ref(cv::Mat &mat, QImage::Format format)
{
    return QImage(mat.data, mat.cols, mat.rows, mat.step, format);
}

cv::Mat qimage2matRef(QImage& qimage)
{
    return cv::Mat(qimage.height(), qimage.width(), CV_8UC3, qimage.bits(), qimage.bytesPerLine());
}

cv::Mat qimage2matCopy(QImage& qimage)
{
    return qimage2matRef(qimage).clone();
}

}


OpenCv::OpenCv()
    : _impl(new OpenCvPrivate)
    , _thread(new QThread)
{
    connect(_impl, &OpenCvPrivate::imageChanged,   this, &OpenCv::imageChanged, Qt::QueuedConnection);

    connect(_impl, &OpenCvPrivate::blobChanged,   this, &OpenCv::blobChanged, Qt::QueuedConnection);
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

void OpenCv::blobDetector(QImage img)
{
    QMetaObject::invokeMethod(_impl, "blobDetector", Qt::QueuedConnection, Q_ARG(QImage, img));
}


OpenCvPrivate::OpenCvPrivate()
{
    db().insert("blob_minArea", 5000);
    db().insert("blob_maxArea", 30000);
//    db().insert("circle_param1", 168);
//    db().insert("circle_param2", 29);
//    db().insert("circle_minRadius", 80);
//    db().insert("circle_maxRadius", 110);


    db().insert("circle_dp", 1.2);
    db().insert("circle_minDist", 70);
    db().insert("circle_param1", 168);
    db().insert("circle_param2", 29);
    db().insert("circle_minRadius", 80);
    db().insert("circle_maxRadius", 110);
}

OpenCvPrivate::~OpenCvPrivate()
{

}

void OpenCvPrivate::init()
{

}

void OpenCvPrivate::searchCircles(QImage img)
{
    if (!_jobDone.tryLock())
        return;

    try
    {
        //ScopedMeasure m("all");
        cv::Mat rgbimg = qimage2matRef(img);
        cv::Mat grey;
        {
            //ScopedMeasure m("color");
            cv::cvtColor(rgbimg, grey, cv::COLOR_RGB2GRAY);
        }

        cv::Mat blur;
        cv::medianBlur(grey, blur, 3);

        std::vector<cv::Vec3f> circles;
        {
            //ScopedMeasure m("circles");

            double dp = db().value("circle_dp").toDouble();
            double minDist = db().value("circle_minDist").toDouble();
            double param1 = db().value("circle_param1").toDouble();
            double param2 = db().value("circle_param2").toDouble();
            int minRadius = db().value("circle_minRadius").toInt();
            int maxRadius = db().value("circle_maxRadius").toInt();

            cv::HoughCircles(blur, circles, cv::HOUGH_GRADIENT, dp, minDist, param1, param2, minRadius, maxRadius);
        }

        drawCircles(rgbimg, circles);

        QImage im = QImage(rgbimg.data, rgbimg.cols, rgbimg.rows, QImage::Format_RGB888);

        emit imageChanged(im);
    }
    catch (...)
    {
        qd() << "упало";
    }

    _jobDone.unlock();
}

void OpenCvPrivate::blobDetector(QImage img)
{
    if (!_jobDone.tryLock())
        return;

    // Setup BlobDetector
    cv::SimpleBlobDetector::Params params;

    // Filter by Area.
    params.filterByArea = true;
    params.minArea = db().value("blob_minArea").toFloat();
    params.maxArea = db().value("blob_maxArea").toFloat();

    // Filter by Circularity
    params.filterByCircularity = true;
    params.minCircularity = 0.5;
    params.maxCircularity = 5.0;

    // Filter by Convexity
    //params.filterByConvexity = false;
    //params.minConvexity = 0.87

    // Filter by Inertia
    params.filterByInertia = true;
    params.minInertiaRatio = 0.8;
    params.maxInertiaRatio = 5.0;

    // Distance Between Blobs
    params.minDistBetweenBlobs = 2.0;

    params.thresholdStep = 10.0;
    params.minThreshold = 1.0;
    params.maxThreshold = 200.0;

    // Create a detector with the parameters
    cv::Ptr<cv::SimpleBlobDetector> detector = cv::SimpleBlobDetector::create(params);

    // Detect blobs
    cv::Mat rgbimg = qimage2matRef(img);
    cv::Mat grey;
    cv::cvtColor(rgbimg, grey, cv::COLOR_RGB2GRAY);

    // Storage for blobs
    std::vector<cv::KeyPoint> keypoints;
    detector->detect(grey, keypoints);

    drawKeyPoints(rgbimg, keypoints);

    QImage im = QImage(rgbimg.data, rgbimg.cols, rgbimg.rows, QImage::Format_RGB888);

    emit blobChanged(im);

    _jobDone.unlock();
}
