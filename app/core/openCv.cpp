#include "openCv.h"
#include "utils.h"

#include "data_bus.h"

#include <QtConcurrent>

#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>

namespace ColorRgb {
cv::Scalar Blue(0, 0, 255, 255);
cv::Scalar Red(255, 0, 0, 255);
cv::Scalar Green(0, 255, 0, 255);
cv::Scalar White(255, 255, 255);
cv::Scalar Black(0, 0, 0, 0);
}

namespace {

// frameCenterPos - позиция центра изображения. Позиция находится между центральными пикселями.
// pixPos - [0, pixelInLine)
double pixToRealX(double frameCenterPos, double pixPos, int pixelInLine)
{
    // При 4 пикселях ширине изображения, координата 2,1 находится в положительной части. 1,9 в отрицательной, относительно
    // центра.
    const double pixelSize = db().value("pixel_size").toDouble(); // мм
    const double posOriginRelativeCenter = pixPos - (pixelInLine / 2);
    return frameCenterPos + (posOriginRelativeCenter * pixelSize);
}


double pixToRealY(double frameCenterPos, double pixPos, int pixelInLine)
{
    // При 4 пикселях ширине изображения, координата 2,1 находится в положительной части. 1,9 в отрицательной, относительно
    // центра.
    const double pixelSize = db().value("pixel_size").toDouble(); // мм
    const double posOriginRelativeCenter = pixPos - (pixelInLine / 2);
    return frameCenterPos - (posOriginRelativeCenter * pixelSize);
}

// В нижнем левом углу
void drawTextBottomLeft(const cv::Mat& image, const QString& text)
{
    cv::putText(image, text.toLatin1().toStdString(),cv::Point(0, image.rows-5), // Чуть повыше текст, на 5 линий
                cv::FONT_HERSHEY_DUPLEX,  2.0, ColorRgb::Blue, 2, cv::LINE_AA);
}

void drawCircles(const cv::Mat& image, const std::vector<cv::Vec3f>& circles)
{
    for (const cv::Vec3f& c : circles)
    {
        const cv::Point center = cv::Point(c[0], c[1]);
        cv::circle(image, center, 1, ColorRgb::Blue, 3, cv::LINE_AA);     // circle center
        cv::circle(image, center, c[2], ColorRgb::Green, 1, cv::LINE_AA); // circle outline
    }
}

void drawKeyPoints(const cv::Mat& image, const std::vector<cv::KeyPoint>& kps)
{
    for (const cv::KeyPoint & kp : kps)
    {
        cv::circle(image, kp.pt, 1, ColorRgb::Blue, 3, cv::LINE_AA);          // circle center
        cv::circle(image, kp.pt, kp.size/2, ColorRgb::Red, -1, cv::LINE_AA); // circle outline
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

// Опасная функция. Внутренний буфер разделен между QImage и Mat
cv::Mat qimage2matRef(const QImage& qimage)
{
    return cv::Mat(qimage.height(), qimage.width(), CV_8UC3, const_cast<uchar *>(qimage.constBits()), qimage.bytesPerLine());
}

cv::Mat qimage2matCopy(const QImage& qimage)
{
    return qimage2matRef(qimage).clone();
}

// img должно быть скопировано, при передаче в эту функцию
// Круги рисуются прямо на переданном изображении, без копирования.
OpenCv::BlobInfo detectBlobs(QImage img)
{
    ScopedMeasure mes ("blob detector", ScopedMeasure::Milli);

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

    params.thresholdStep = db().value("blob_thresholdStep").toFloat();
    params.minThreshold = db().value("blob_minThreshold").toFloat();
    params.maxThreshold = db().value("blob_maxThreshold").toFloat();

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

    //QImage im = QImage(rgbimg.data, rgbimg.cols, rgbimg.rows, QImage::Format_RGB888);

    return {img, keypoints};
}

}


OpenCv::OpenCv()
    : _impl(new OpenCvPrivate)
    , _thread(new QThread)
{
    connect(_impl, &OpenCvPrivate::circleChanged,   this, &OpenCv::circleChanged, Qt::QueuedConnection);
    connect(_impl, &OpenCvPrivate::blobChanged,   this, &OpenCv::blobChanged, Qt::QueuedConnection);

    connect(_thread.data(), &QThread::finished, _impl, &QObject::deleteLater);
    connect(_thread.data(), &QThread::started,  _impl, &OpenCvPrivate::init, Qt::QueuedConnection);

    _impl->moveToThread(_thread.data());
    _thread->start();


    connect(&_blobWatcherCaptured, &QFutureWatcher<OpenCv::BlobInfo>::finished, this, [this]()
    {
        QImage im = std::get<0>(_blobWatcherCaptured.result());
        std::vector<cv::KeyPoint> kps = std::get<1>(_blobWatcherCaptured.result());

        if (kps.empty())
            return;

        QString x = im.text("x");
        QString y = im.text("y");

        _detectBlobResult.push_back({kps, x, y, im.width(), im.height()});
        foundBlobs();
    });

    QTimer* timer = new QTimer;
    timer->start(50);

    connect(timer, &QTimer::timeout, this, [this]()
    {
        if (!_detectBlobQueue.isEmpty() && _blobWatcherCaptured.isFinished())
        {
            QImage img = _detectBlobQueue.first();
            _detectBlobQueue.pop_front();
            QFuture<OpenCv::BlobInfo> future = QtConcurrent::run(detectBlobs, img);
            _blobWatcherCaptured.setFuture(future);
        }
    });


    db().insert("pixel_size", 0.00524);
}

OpenCv::~OpenCv()
{
    _thread->quit();
    _thread->wait(1000);
}

void OpenCv::searchCirclesLive(QImage img)
{
    QMetaObject::invokeMethod(_impl, "searchCirclesLive", Qt::QueuedConnection, Q_ARG(QImage, img));
}

void OpenCv::blobDetectorLive(QImage img)
{
    QMetaObject::invokeMethod(_impl, "blobDetectorLive", Qt::QueuedConnection, Q_ARG(QImage, img));
}

void OpenCv::blobDetectorCaptured(QImage img)
{
    _detectBlobQueue.push_back(img);
}

void OpenCv::blobDetectorUpdated(QImage img)
{
    if (!_blobWatcherCapturedSmallRegion.isFinished())
        return;

    QFuture<OpenCv::BlobInfo> future = QtConcurrent::run(detectBlobs, img);
    _blobWatcherCapturedSmallRegion.setFuture(future);

    disconnect(_smallRegConn);

    _smallRegConn= connect(&_blobWatcherCapturedSmallRegion, &QFutureWatcher<OpenCv::BlobInfo>::finished, this, [this]()
    {
        QImage im = std::get<0>(_blobWatcherCapturedSmallRegion.result());
        std::vector<cv::KeyPoint> kps = std::get<1>(_blobWatcherCapturedSmallRegion.result());

        if (kps.empty())
            return;

        QString x = im.text("x");
        QString y = im.text("y");

        auto kp = kps[0];

        const double xBlob = pixToRealX(x.toDouble(), kp.pt.x, im.width());
        const double yBlob = pixToRealY(y.toDouble(), kp.pt.y, im.height());

        _smallRegionBlob = QString("%1 %2").arg(QString::number(xBlob, 'f', 3)).arg(QString::number(yBlob, 'f', 3));

        emit smallRegionBlobChanged();
    });


}

QString OpenCv::smallRegionBlob() const
{
    return _smallRegionBlob;
}

// Текст рисуется на переданном изображение. И возвращается оно же просто для удобства.
QImage OpenCv::drawText(const QImage& img, const QString& text)
{
    cv::Mat image = qimage2matRef(img);
    ::drawTextBottomLeft(image, text);
    return img;
}

QImage OpenCv::drawCross(const QImage& img)
{
    cv::Mat image = qimage2matRef(img);

    cv::Point a1((image.cols / 2) - 2, 0);
    cv::Point a2((image.cols / 2) - 2, image.rows - 1);

    cv::Point b1((image.cols / 2) + 1, 0);
    cv::Point b2((image.cols / 2) + 1, image.rows - 1);

    cv::Point c1(0,              (image.rows / 2) - 2);
    cv::Point c2(image.cols - 1, (image.rows / 2) - 2);

    cv::Point d1(0,              (image.rows / 2) + 1);
    cv::Point d2(image.cols - 1, (image.rows / 2) + 1);

    cv::line(image, a1, a2, ColorRgb::White);
    cv::line(image, b1, b2, ColorRgb::White);
    cv::line(image, c1, c2, ColorRgb::White);
    cv::line(image, d1, d2, ColorRgb::White);

    return img;
}

void OpenCv::foundBlobs() const
{
    QStringList s;
    for (const auto& result : _detectBlobResult)
    {
        auto kps = std::get<0>(result);
        QString x = std::get<1>(result);
        QString y = std::get<2>(result);
        s.append(QString("[%1 %2]").arg(x, y));

        for (const cv::KeyPoint& kp : kps)
        {
            s.append(QString("\t size: %1 pos: [%2 %3]").arg(kp.size).arg(kp.pt.x).arg(kp.pt.y));
        }
    }
    db().insert("found_blobs", s.join("\n"));


    s.clear();
    for (const auto& result : _detectBlobResult)
    {
        auto kps = std::get<0>(result);
        QString x = std::get<1>(result);
        QString y = std::get<2>(result);

        int w = std::get<3>(result);
        int h = std::get<4>(result);

        for (const cv::KeyPoint& kp : kps)
        {
            s.append(QString("size: %1 pos: [%2 %3]").arg(kp.size)
                     .arg(pixToRealX(x.toDouble(), kp.pt.x, w))
                     .arg(pixToRealY(y.toDouble(), kp.pt.y, h)));
        }
    }

    db().insert("found_blobs2", s.join("\n"));


    s.clear();
    for (const BlobInfo2& result : _detectBlobResult)
    {
        auto kps = std::get<0>(result);

        const QString imX = std::get<1>(result);
        const QString imY = std::get<2>(result);

        const int imWidth = std::get<3>(result);
        const int imHeight = std::get<4>(result);

        for (const cv::KeyPoint& kp : kps)
        {
            const double xBlob = pixToRealX(imX.toDouble(), kp.pt.x, imWidth);
            const double yBlob = pixToRealY(imY.toDouble(), kp.pt.y, imHeight);
            s.append(QString("%1 %2").arg(QString::number(xBlob, 'f', 3)).arg(QString::number(yBlob, 'f', 3)));
        }
    }

    db().insert("found_blobs3", s);
}

void OpenCv::resetFoundBlobs()
{
    _detectBlobResult.clear();
}

OpenCvPrivate::OpenCvPrivate()
{
    db().insert("blob_minArea", 1000);
    db().insert("blob_maxArea", 100000);
    db().insert("blob_thresholdStep", 10);
    db().insert("blob_minThreshold", 1);
    db().insert("blob_maxThreshold", 200);
    //    db().insert("circle_maxRadius", 110);

    db().insert("blob_info", "");


    db().insert("circle_dp", 1.2);
    db().insert("circle_minDist", 70);
    db().insert("circle_param1", 168);
    db().insert("circle_param2", 29);
    db().insert("circle_minRadius", 80);
    db().insert("circle_maxRadius", 110);

    connect(&_circleWatcherLive, &QFutureWatcher<QImage>::finished, this, [this]()
    {
        emit circleChanged(_circleWatcherLive.result());
    });

    connect(&_blobWatcherLive, &QFutureWatcher<OpenCv::BlobInfo>::finished, this, [this]()
    {
        QImage im = std::get<0>(_blobWatcherLive.result());
        emit blobChanged(im);

        std::vector<cv::KeyPoint> kps = std::get<1>(_blobWatcherLive.result());

        QString res;
        for (cv::KeyPoint kp : kps)
        {
            res += QString("(%1, %2) \t %3\n").arg(kp.pt.x).arg(kp.pt.y).arg(kp.size);
        }

        db().insert("blob_info", res);


        QString x = im.text("x");
        QString y = im.text("y");

        res.clear();
        for (const cv::KeyPoint& kp : kps)
        {
            QString xBlob = QString::number(pixToRealX(x.toDouble(), kp.pt.x, im.width()), 'f', 3);
            QString yBlob = QString::number(pixToRealY(y.toDouble(), kp.pt.y, im.height()),'f', 3);

            res.append(QString("size: %1 pos: [%2 %3] \n").arg(kp.size).arg(xBlob).arg(yBlob));
        }
        db().insert("blob_info2", res);


        res.clear();
        if (!kps.empty())
        {
            auto kp = kps[0];
            QString xBlob = QString::number(pixToRealX(x.toDouble(), kp.pt.x, im.width()), 'f', 3);
            QString yBlob = QString::number(pixToRealY(y.toDouble(), kp.pt.y, im.height()),'f', 3);

            res = QString("%1 %2").arg(xBlob).arg(yBlob);
        }
        db().insert("blob_info3", res);


        res.clear();
        if (!kps.empty())
        {
            auto kp = kps[0];
            double xBlob = pixToRealX(x.toDouble(), kp.pt.x, im.width());
            double yBlob = pixToRealY(y.toDouble(), kp.pt.y, im.height());

            res = QString("%1 %2").arg(x.toDouble() - xBlob).arg(y.toDouble() - yBlob);
        }
        db().insert("blob_info4", res);

    });
}

void OpenCvPrivate::init()
{

}

void OpenCvPrivate::searchCirclesLive(QImage img)
{
    if (!_circleWatcherLive.isFinished())
        return;

    QFuture<QImage> future = QtConcurrent::run(this, &OpenCvPrivate::searchCirclesWorker, img);
    _circleWatcherLive.setFuture(future);
}

// img должно быть скопировано, при передаче в эту функцию
// Круги рисуются прямо на переданном изображении, без копирования.
QImage OpenCvPrivate::searchCirclesWorker(QImage img)
{
    ScopedMeasure mes ("circles", ScopedMeasure::Milli);

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

        //QImage im = QImage(rgbimg.data, rgbimg.cols, rgbimg.rows, QImage::Format_RGB888);

        return img;
    }
    catch (...)
    {
        qd() << "упало";
    }
    return QImage();
}

void OpenCvPrivate::blobDetectorLive(QImage img)
{
    if (!_blobWatcherLive.isFinished())
        return;

    QFuture<OpenCv::BlobInfo> future = QtConcurrent::run(detectBlobs, img);
    _blobWatcherLive.setFuture(future);
}

