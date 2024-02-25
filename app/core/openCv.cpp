#include "openCv.h"
#include "utils.h"

#include "data_bus.h"
#include "scene.h"
#include "settings.h"

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

// void Rearrange(cv::Mat& src, cv::Mat& dst)
// {
//     int cx = src.cols / 2;
//     int cy = src.rows / 2;
//     cv::Mat tmp;
//     tmp.create(src.size(), src.type());
//     src(cv::Rect(0, 0, cx, cy)).copyTo(tmp(cv::Rect(cx, cy, cx, cy)));
//     src(cv::Rect(cx, cy, cx, cy)).copyTo(tmp(cv::Rect(0, 0, cx, cy)));
//     src(cv::Rect(cx, 0, cx, cy)).copyTo(tmp(cv::Rect(0, cy, cx, cy)));
//     src(cv::Rect(0, cy, cx, cy)).copyTo(tmp(cv::Rect(cx, 0, cx, cy)));
//     dst = tmp;
// }

// cv::Mat XCorrelation(cv::Mat const& I, cv::Mat const& I1)
// {
//     int width = cv::getOptimalDFTSize(std::max(I.cols,I1.cols));
//     int height = cv::getOptimalDFTSize(std::max(I.rows,I1.rows));
//     cv::Mat fft1;
//     cv::Mat fft2;

//     cv::copyMakeBorder(I, fft1, 0, height - I.rows, 0, width - I.cols, cv::BORDER_CONSTANT, cv::Scalar::all(0));
//     cv::copyMakeBorder(I1, fft2, 0, height - I.rows, 0, width - I.cols, cv::BORDER_CONSTANT, cv::Scalar::all(0));

//     fft1.convertTo(fft1, CV_32F);
//     fft2.convertTo(fft2, CV_32F);

//     cv::dft(fft1,fft1,0,I.rows);
//     cv::dft(fft2,fft2,0,I1.rows);

//     cv::mulSpectrums(fft1,fft2,fft1,0,true);
//     // here cv::DFT_SCALE divide `width*height` 1 times
//     cv::idft(fft1,fft1,cv::DFT_SCALE|cv::DFT_REAL_OUTPUT);
//     Rearrange(fft1, fft1);
//     // here divide another times
//     return cv::abs(fft1)/(width*height);

//     //    Mat  draw;
//     //    sobelx.convertTo(draw,  CV_8U,  255.0/(maxVal  -  minVal),  -minVal);
// }

// frameCenterPos - позиция центра изображения. Позиция находится между центральными пикселями.
// pixPos - [0, pixelInLine)
double pixToRealX(double frameCenterPos, double pixPos, int pixelInLine)
{
    // При 4 пикселях ширине изображения, координата 2,1 находится в положительной части. 1,9 в отрицательной, относительно
    // центра.
    const double posOriginRelativeCenter = pixPos - (pixelInLine / 2);
    return frameCenterPos + (posOriginRelativeCenter / db().pixInMm());
}


double pixToRealY(double frameCenterPos, double pixPos, int pixelInLine)
{
    // При 4 пикселях ширине изображения, координата 2,1 находится в положительной части. 1,9 в отрицательной, относительно
    // центра.
    const double posOriginRelativeCenter = pixPos - (pixelInLine / 2);
    return frameCenterPos - (posOriginRelativeCenter / db().pixInMm());
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

cv::Mat adaptiveThreshold(const cv::Mat& in)
{
    cv::Mat out;

    const bool adTrEnable = db().value("blob_ad_tr_enable").toBool();
    if (adTrEnable)
    {
        const int blockSize = db().value("blob_ad_tr_blockSize").toInt();
        const double c = db().value("blob_ad_tr_c").toDouble();
        const int typeAdTr = db().value("blob_ad_tr_type").toInt(); // 0, 1
        const int typeTr = db().value("blob_tr_type").toInt(); // 0, 1, 2,3,4,7,8,16
        cv::adaptiveThreshold(in, out, 255, typeAdTr, typeTr, blockSize, c);
    }
    else
    {
        out = in;
    }

    return out;
}




// img должно быть скопировано, при передаче в эту функцию
// Круги рисуются прямо на переданном изображении, без копирования.
OpenCv::BlobInfo detectBlobs(QImage img)
{
    static quint32 count = 0;
    ++count;

    try {

        //ScopedMeasure mes (QString("blob detect (%1) ").arg(count), ScopedMeasure::Milli);

        //qd() << "detect blobs " << img.width() << img.height();

        cv::SimpleBlobDetector::Params params;

        // Filter by Area.
        params.filterByArea = true;
        double minDia = db().value("blob_minDia_mm").toDouble();
        double maxDia = db().value("blob_maxDia_mm").toDouble();

        params.minArea = minDia * minDia * 3.14159 * db().pixInMm() * db().pixInMm() / 4;
        params.maxArea = maxDia * maxDia * 3.14159 * db().pixInMm() * db().pixInMm() / 4;

        // Filter by Circularity
        params.filterByCircularity = false;
        //params.minCircularity = 0.01;
        //params.maxCircularity = 5.0;

        // Filter by Convexity
        params.filterByConvexity = db().value("blob_filter_convexity_enabled").toBool();
        params.minConvexity = db().value("blob_filter_convexity_min").toDouble();
        params.maxConvexity = db().value("blob_filter_convexity_max").toDouble();

        // Filter by Inertia
        params.filterByInertia = false;
        //    params.minInertiaRatio = 0.8;
        //    params.maxInertiaRatio = 5.0;

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

        cv::Mat blur;
        cv::medianBlur(grey, blur, 5);
        //cv::GaussianBlur(adtr, blur, cv::Size(19, 19), 0, 0, cv::BORDER_CONSTANT);

        cv::Mat adTr = adaptiveThreshold(blur);


        //db().insert("image_adapt_threshold_1", mat_to_qimage_ref(adtr, QImage::Format_Alpha8).copy());
        db().insert("image_adapt_threshold_2", mat_to_qimage_ref(adTr, QImage::Format_Grayscale8).copy());

        // cv::Mat blur;
        // cv::medianBlur(adtr, blur, 3);
        //cv::GaussianBlur(adtr, blur, cv::Size(19, 19), 0, 0, cv::BORDER_CONSTANT);

        // Storage for blobs
        std::vector<cv::KeyPoint> keypoints;
        detector->detect(adTr, keypoints);

        drawKeyPoints(rgbimg, keypoints);

        //QImage im = QImage(rgbimg.data, rgbimg.cols, rgbimg.rows, QImage::Format_RGB888);

        return {img, keypoints};
    }
    catch (...)
    {
        const int typeAdTr = db().value("blob_ad_tr_type").toInt(); // 0, 1
        const int typeTr = db().value("blob_tr_type").toInt(); // 0, 1, 2,3,4,7,8,16
        qd() << "blob crashed" << typeAdTr << typeTr;
    }

    return {img, std::vector<cv::KeyPoint>()};
}

}


OpenCv::OpenCv()
    : _impl(new OpenCvPrivate)
    , _thread(new QThread)
{
    connect(_impl, &OpenCvPrivate::circleChanged,   this, &OpenCv::circleChanged, Qt::QueuedConnection);
    connect(_impl, &OpenCvPrivate::blobChanged,   this, &OpenCv::blobChanged, Qt::QueuedConnection);
    connect(_impl, &OpenCvPrivate::smallRegionBlobImage,   this, &OpenCv::smallRegionBlobImage, Qt::QueuedConnection);

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

        placeFoundBlobsOnScene({kps, x, y, im.width(), im.height()});
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
}

OpenCv::~OpenCv()
{
    _thread->quit();
    _thread->wait(1000);
}

double OpenCv::corr(QImage cap1, QImage cap2)
{
    cv::Mat img1 = cv::Mat(cap1.height(), cap1.width(), CV_8UC3, const_cast<uchar *>(cap1.constBits()), cap1.bytesPerLine());
    cv::Mat img2 = cv::Mat(cap2.height(), cap2.width(), CV_8UC3, const_cast<uchar *>(cap2.constBits()), cap2.bytesPerLine());

    cv::Mat gr1;
    cv::Mat gr2;
    cv::cvtColor(img1, gr1, cv::COLOR_RGB2GRAY);
    cv::cvtColor(img2, gr2, cv::COLOR_RGB2GRAY);

    gr1.convertTo(gr1, CV_32F);
    gr2.convertTo(gr2, CV_32F);

    //    cv::Mat res = XCorrelation(gr1, gr2);
    //    db().insert("image_corr", mat_to_qimage_ref(res, QImage::Format_Grayscale8).copy());

    cv::Point2d res = cv::phaseCorrelate(gr1, gr2);

    qd() << " result " << res.x << res.y;

    return res.x;
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

    _smallRegConn = connect(&_blobWatcherCapturedSmallRegion, &QFutureWatcher<OpenCv::BlobInfo>::resultReadyAt, this, [this](int index)
    {
        OpenCv::BlobInfo result = _blobWatcherCapturedSmallRegion.resultAt(index);

        QImage im = std::get<0>(result);
        std::vector<cv::KeyPoint> kps = std::get<1>(result);

        if (kps.empty())
        {
            qd() << "kps is empty";
            _smallRegionBlob = {false, 0, 0, 0};
            emit smallRegionBlobDetectionFinished();
            return;
        }


        emit smallRegionBlobImage(im.copy());

        auto kp = kps[0];

        qd() << "kp size " << kps.size();

        //qd() << "kp size " << kp.size;
        //qd() << "smakl region width " << im.width();

        QString x = im.text("x");
        QString y = im.text("y");
        const double xBlob = pixToRealX(x.toDouble(), kp.pt.x, im.width());
        const double yBlob = pixToRealY(y.toDouble(), kp.pt.y, im.height());
        const double diaBlob = kp.size / db().pixInMm();

        _smallRegionBlob = {true, xBlob, yBlob, diaBlob};

        const double xBlobError = pixToRealX(0.0, kp.pt.x, im.width());
        const double yBlobError = pixToRealY(0.0, kp.pt.y, im.height());

        qd() << "blob error " << xBlobError << yBlobError;

        emit smallRegionBlobDetectionFinished();
    });
}

std::tuple<bool, double, double, double> OpenCv::smallRegionBlob() const
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

void OpenCv::placeFoundBlobsOnScene(const BlobInfo2& blobs) const
{
    auto kps = std::get<0>(blobs);

    const QString imX = std::get<1>(blobs);
    const QString imY = std::get<2>(blobs);

    const int imWidth = std::get<3>(blobs);
    const int imHeight = std::get<4>(blobs);

    for (const cv::KeyPoint& kp : kps)
    {
        const double xBlob = pixToRealX(imX.toDouble(), kp.pt.x, imWidth);
        const double yBlob = pixToRealY(imY.toDouble(), kp.pt.y, imHeight);
        const double diaBlob = kp.size / db().pixInMm();

        scene().addBlob(xBlob, yBlob, diaBlob);
    }
}

OpenCvPrivate::OpenCvPrivate()
{
    db().insert("blob_minDia_mm", settings().value("blob_minDia_mm", 0.3).toDouble());
    db().insert("blob_maxDia_mm", settings().value("blob_maxDia_mm", 6.0).toDouble());
    db().insert("blob_thresholdStep", 10);
    db().insert("blob_minThreshold", 1);
    db().insert("blob_maxThreshold", 200);
    //    db().insert("circle_maxRadius", 110);

    db().insert("blob_ad_tr_blockSize", settings().value("blob_ad_tr_blockSize", 29).toInt());
    db().insert("blob_ad_tr_c",         settings().value("blob_ad_tr_c", 9.0).toDouble());

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
        res += "x_pix \t y_pix \t dia_pix \t x_mm \t y_mm \t dia_mm \t dx_mm \t dy_mm \n";

        QString x = im.text("x");
        QString y = im.text("y");

        for (const cv::KeyPoint& kp : kps)
        {
            const double xBlob = pixToRealX(x.toDouble(), kp.pt.x, im.width());
            const double yBlob = pixToRealY(y.toDouble(), kp.pt.y, im.height());
            const double diaBlob = kp.size / db().pixInMm();

            res.append(QString("%1 \t %2 \t %3 \t %4 \t %5 \t %6 \t %7 \t %8 \n")
                       .arg(toReal1(kp.pt.x))
                       .arg(toReal1(kp.pt.y))
                       .arg(toReal1(kp.size))
                       .arg(toReal3(xBlob))
                       .arg(toReal3(yBlob))
                       .arg(toReal3(diaBlob))
                       .arg(toReal3(xBlob - x.toDouble())) // дельта между блобом и центром
                       .arg(toReal3(yBlob - y.toDouble()))); // дельта между блобом и центром
        }
        db().insert("blob_info", res);

        //        res.clear();
        //        if (!kps.empty())
        //        {
        //            auto kp = kps[0];
        //            double xBlob = pixToRealX(x.toDouble(), kp.pt.x, im.width());
        //            double yBlob = pixToRealY(y.toDouble(), kp.pt.y, im.height());

        //            res = QString("%1 %2").arg(x.toDouble() - xBlob).arg(y.toDouble() - yBlob);
        //        }
        //        db().insert("blob_info4", res);
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

