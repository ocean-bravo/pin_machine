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

cv::Mat qimage2mat(QImage& qimage)
{
    cv::Mat mat = cv::Mat(qimage.height(), qimage.width(), CV_8UC3, qimage.bits(), qimage.bytesPerLine());
//    cv::Mat mat2 = cv::Mat(mat.rows, mat.cols, CV_8UC3 );
//    int from_to[] = { 0,0,  1,1,  2,2 };
//    cv::mixChannels( &mat, 1, &mat2, 1, from_to, 3 );
//    return mat2;
    return mat.clone();
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

void OpenCv::searchCircles(QImage img, QByteArray ba)
{
    QMetaObject::invokeMethod(_impl, "searchCircles", Qt::QueuedConnection, Q_ARG(QImage, img), Q_ARG(QByteArray, ba));
}

void OpenCv::blobDetector(QImage img, QByteArray ba)
{
    QMetaObject::invokeMethod(_impl, "blobDetector", Qt::QueuedConnection, Q_ARG(QImage, img), Q_ARG(QByteArray, ba));
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

void OpenCvPrivate::searchCircles(QImage imag, QByteArray ba)
{
    if (!_jobDone.tryLock())
        return;

    QImage img = imag.copy();

    static int i = 0;
    static int fmtOut = 0;
    static int fmtIn = 107;
    static std::vector<int> formatsIn = {CV_16UC1, CV_16UC2, CV_16UC3,CV_16UC4};
//    ,
//                                     CV_8SC1, CV_8SC2, CV_8SC3, CV_8SC4,   CV_8UC1, CV_8UC2, CV_8UC3, CV_8UC4
//                                     CV_16UC1, CV_16UC2, CV_16UC3,CV_16UC4};

    ++i;
    if (i % 20 == 0) ++fmtOut;
    if (fmtOut == 30) fmtOut = 0;
    if (i % 20 == 0) ++fmtIn;
    if (fmtIn == 126) fmtIn = 107;
//    qd() << "in format " << fmtIn;
//    qd() << "out format " << fmtOut;
    try
    {
        //ScopedMeasure m("all");
        cv::Mat rgbimg = qimage2mat(img);
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
            //cv::HoughCircles(blur, circles, cv::HOUGH_GRADIENT, dp, 70, 168, 29, 80, 110);
        }

        drawCircles(rgbimg, circles);

        QImage im1 = QImage(rgbimg.data, rgbimg.cols, rgbimg.rows, QImage::Format_RGB888);

        im1.detach();
        emit imageChanged(im1);
    }
    catch (...)
    {
        qd() << "упало";
    }

    _jobDone.unlock();
}

void OpenCvPrivate::blobDetector(QImage imag, QByteArray ba)
{
    if (!_jobDone.tryLock())
        return;

    QImage img = imag.copy();

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
    cv::Mat rgbimg = qimage2mat(img);
    cv::Mat grey;
    cv::cvtColor(rgbimg, grey, cv::COLOR_RGB2GRAY);

    // Storage for blobs
    std::vector<cv::KeyPoint> keypoints;
    detector->detect(grey, keypoints);

    // Draw detected blobs as red circles.
    // DrawMatchesFlags::DRAW_RICH_KEYPOINTS flag ensures
    // the size of the circle corresponds to the size of blob

    cv::Mat im_with_keypoints;
    //cv::drawKeypoints(grey, keypoints, im_with_keypoints, Color::ColorRed, cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
    drawKeyPoints(rgbimg, keypoints);

//    emit blobChanged(mat_to_qimage_ref(im_with_keypoints, QImage::Format_BGR888));



    //QImage im1 = QImage(im_with_keypoints.data, im_with_keypoints.cols, im_with_keypoints.rows, QImage::Format_RGB888);
    QImage im1 = QImage(rgbimg.data, rgbimg.cols, rgbimg.rows, QImage::Format_RGB888);

    im1.detach();
    emit blobChanged(im1);

    _jobDone.unlock();

//while camera.isOpened():

//    retval, im = camera.read()
//    overlay = im.copy()

//    keypoints = detector.detect(im)
//    for k in keypoints:
//        cv2.circle(overlay, (int(k.pt[0]), int(k.pt[1])), int(k.size/2), (0, 0, 255), -1)
//        cv2.line(overlay, (int(k.pt[0])-20, int(k.pt[1])), (int(k.pt[0])+20, int(k.pt[1])), (0,0,0), 3)
//        cv2.line(overlay, (int(k.pt[0]), int(k.pt[1])-20), (int(k.pt[0]), int(k.pt[1])+20), (0,0,0), 3)

//    opacity = 0.5
//    cv2.addWeighted(overlay, opacity, im, 1 - opacity, 0, im)

//    # Uncomment to resize to fit output window if needed
//    #im = cv2.resize(im, None,fx=0.5, fy=0.5, interpolation = cv2.INTER_CUBIC)
//    cv2.imshow("Output", im)

}
