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

void OpenCvPrivate::searchCircles(QImage img, QByteArray ba)
{
    if (_done)
    {
        _done = false;
    }
    else
    {
        return;
    }

    static int i = 0;
    static int fmtOut = 0;
    static int fmtIn = 0;
    static std::vector<int> formatsIn = {CV_16UC1, CV_16UC2, CV_16UC3,CV_16UC4};
//    ,
//                                     CV_8SC1, CV_8SC2, CV_8SC3, CV_8SC4,   CV_8UC1, CV_8UC2, CV_8UC3, CV_8UC4
//                                     CV_16UC1, CV_16UC2, CV_16UC3,CV_16UC4};

    ++i;

    if (i % 20 == 0)
        ++fmtOut;

    if (fmtOut == 30)
        fmtOut = 0;


//    if (i % 5 == 0)
//        ++fmtIn;

//    if (fmtIn == 4)
//        fmtIn = 0;


    qd() << "in format " << fmtIn;
    qd() << "out format " << fmtOut;


    try
    {
        //cv::Mat image = qimage_to_mat_cpy(img,  CV_8UC3);CV_8UC2

        cv::Mat image = cv::Mat(cv::Size(img.width(),img.height()), CV_8UC2 , ba.data());
        cv::Mat rgbimg;
        cv::Mat grey;
        {
            ScopedMeasure m("color");
//            cv::cvtColor(image, rgbimg, cv::COLOR_YUV2RGB_UYVY);
//            cv::cvtColor(image, grey, cv::COLOR_BGR2GRAY);

            cv::cvtColor(image, rgbimg, cv::COLOR_YUV2BGR_YUY2);
            cv::cvtColor(rgbimg, grey, cv::COLOR_BGR2GRAY);
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

        drawCircles(rgbimg, circles);

        //cv::imshow("main", image);
        //cv::resizeWindow("main", cv::Size(image.size().width / 4, image.size().height / 4));

        emit imageChanged(mat_to_qimage_ref(rgbimg, QImage::Format_BGR888));
    }
    catch (...)
    {
        qd() << "упало";
    }

    _done = true;
}

void OpenCvPrivate::blobDetector(QImage img, QByteArray ba)
{
    return;
    // Setup BlobDetector
    cv::SimpleBlobDetector::Params params;

    // Filter by Area.
    params.filterByArea = true;
    params.minArea = 20000;
    params.maxArea = 40000;

    // Filter by Circularity
    params.filterByCircularity = true;
    params.minCircularity = 0.5;

    // Filter by Convexity
    params.filterByConvexity = false;
    //params.minConvexity = 0.87

    // Filter by Inertia
    params.filterByInertia = true;
    params.minInertiaRatio = 0.8;

    // Distance Between Blobs
    params.minDistBetweenBlobs = 200;

    // Create a detector with the parameters
    cv::Ptr<cv::SimpleBlobDetector> detector = cv::SimpleBlobDetector::create(params);

    // Storage for blobs
    std::vector<cv::KeyPoint> keypoints;

    // Detect blobs
    //
    cv::Mat image = qimage_to_mat_cpy(img,  CV_8UC3);
    cv::Mat grey;
    cv::cvtColor(image, grey, cv::COLOR_BGR2GRAY);
    detector->detect(grey, keypoints);

    // Draw detected blobs as red circles.
        // DrawMatchesFlags::DRAW_RICH_KEYPOINTS flag ensures
        // the size of the circle corresponds to the size of blob

    cv::Mat im_with_keypoints;
    cv::drawKeypoints(grey, keypoints, im_with_keypoints, cv::Scalar(0, 0, 255), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);

    emit blobChanged(mat_to_qimage_ref(im_with_keypoints, QImage::Format_BGR888));

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
