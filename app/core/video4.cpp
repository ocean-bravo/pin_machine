#include "video4.h"
#include "utils.h"
#include "wait.h"

#include "data_bus.h"
#include "utils2.h"

#include <QBuffer>
#include <QEventLoop>
#include <QTimer>
#include <QDateTime>
#include <QScopeGuard>

#include <sys/fcntl.h>
#include <linux/videodev2.h>

#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <signal.h>

#include "my_video_driver.h"
#include "yuvconverters.h"

#include "mjpeghelper.h"

namespace {

}


Video4::Video4()
{
    _impl = new Video4Private;
    _thread = new QThread;

    connect(_impl, &Video4Private::rawImage,   this, &Video4::rawImage, Qt::QueuedConnection);
    connect(_impl, &Video4Private::captured,   this, &Video4::captured, Qt::QueuedConnection);
    connect(_impl, &Video4Private::capturedSmallRegion,  this, [this](QImage img)
    {
        _smallRegion = img;
        emit capturedSmallRegion(img);
    });

    //    connect(_impl, &Video4Private::finished, _impl, &QObject::deleteLater);
    //    connect(_impl, &Video4Private::finished, _thread, &QThread::quit);
    connect(_thread, &QThread::started,  _impl, &Video4Private::init);

    connect(_thread, &QThread::finished, _impl, &QObject::deleteLater);
    connect(_thread, &QThread::finished, _thread, &QObject::deleteLater);

    _impl->moveToThread(_thread);
    _thread->start();


    db().insert("cam_prop_exp_type", QVariant());
    db().insert("cam_prop_exp_time", QVariant());
    db().insert("cam_prop_ape_size", QVariant());

    databusAction2("cam_prop_exp_type", [this](const QVariant& value) { setProperty("exposure_type", value.toUInt()); } );
    databusAction2("cam_prop_exp_time", [this](const QVariant& value) { setProperty("exposure_time", value.toUInt()); } );
    databusAction2("cam_prop_ape_size", [this](const QVariant& value) { setProperty("aperture_size", value.toUInt()); } );
}

Video4::~Video4()
{
    _thread->quit();
    _thread->wait(1000);
}

void Video4::changeCamera(int device, int width, int height, QString fourcc)
{
    QMetaObject::invokeMethod(_impl, "changeCamera", Qt::QueuedConnection, Q_ARG(int, device),Q_ARG(int, width),Q_ARG(int, height),Q_ARG(QString, fourcc));
}

void Video4::start()
{
    _impl->_stop = false;
    QMetaObject::invokeMethod(_impl, "update", Qt::QueuedConnection);
}

void Video4::stop()
{
    _impl->_stop = true;
}

void Video4::capture(double widthMm)
{
    QMetaObject::invokeMethod(_impl, "capture", Qt::QueuedConnection, Q_ARG(double, widthMm));
}

QImage Video4::smallRegion()
{
//    const double x = db().value("xPos").toDouble();
//    const double y = db().value("yPos").toDouble();

//    _smallRegion.setText("x", toReal3(x));
//    _smallRegion.setText("y", toReal3(y));
    return _smallRegion.copy();
}

void Video4::setProperty(const QString& property, qint32 value)
{
    QMetaObject::invokeMethod(_impl, "setProperty", Qt::QueuedConnection, Q_ARG(QString, property), Q_ARG(qint32, value));
}

void Video4::reloadDevices()
{
    QMetaObject::invokeMethod(_impl, "reloadDevices", Qt::QueuedConnection);
}


void Video4Private::init()
{
    _jpegDecompressor = new MjpegHelper;
}

void Video4Private::capture(double widthMm)
{
    int jpgFramesThrow = db().value("jpg_frames_throw").toInt();
    int yuvFramesThrow = db().value("yuv_frames_throw").toInt();

    _framesToThrowOut = _currentFourcc == "YUYV" ? yuvFramesThrow  : jpgFramesThrow;

    if (widthMm == 0.0)
    {
        _rectToCopy = QRect();
        _capture = true;
        return;
    }

    int w = _videoCapture->width;
    int h = _videoCapture->height;

    int widthPix = widthMm * db().pixInMm();

    _rectToCopy = QRect((w/2) - (widthPix/2), (h/2) - (widthPix/2), widthPix, widthPix);
    _captureSmallRegion = true;
}


void Video4Private::reloadDevices()
{
    _videoCapture->reloadDevices();
}

void Video4Private::setProperty(const QString &property, qint32 value)
{
    _videoCapture->setProperty(property, value);
}

void Video4Private::update()
{
    if (!_mutex.tryLock()) return;
    auto mutexUnlock = qScopeGuard([this]{ _mutex.unlock(); });

    std::vector<char> inBuffer;
    inBuffer.reserve(20000000); // должно хватить

    std::vector<char> rgbBuffer;
    rgbBuffer.reserve(20000000); // должно хватить

    auto start = std::chrono::steady_clock::now();
    auto finish = std::chrono::steady_clock::now();

    int i = 0;

    while (true)
    {
        wait(1);

        if (_stop)
            break;

        if (!_videoCapture)
            break;

        if (!_videoCapture->isReady())
            break;

        const bool hasFrame = _videoCapture->isReadable(10);

        if (!hasFrame)
            continue;

        //ScopedMeasure (beginprevline + setpos(85) + "upd ");

        const quint32 buffSize = _videoCapture->bufSize();

        // qd() << "buffSize:" << buffSize;
        {
            //ScopedMeasure ("resize");
            inBuffer.resize(buffSize);
            rgbBuffer.resize(buffSize);
        }

        //qd() << "read internal ...";
        int rsize = _videoCapture->readInternal(inBuffer.data(), buffSize);
        //qd() << "... finished";
        if (rsize == -1)
        {
            qd() << "stop " << strerror(errno);
            break;
        }

        //qd() << "size:" << rsize;
        finish = std::chrono::steady_clock::now();
        const std::chrono::duration<double, std::milli> elapsed = finish - start;
        ++i;
        //qd() << i << ":" << elapsed.count() << "ms";
        //qd() << beginprevline + setpos(60) + QString::number(1000/elapsed.count(), 'f', 1) << "fps";
        start = std::chrono::steady_clock::now();

        {
            //ScopedMeasure ("YUYV2RGB");
            if (_currentFourcc == "YUYV")
                YUYV2RGB((const uint8_t *)inBuffer.data(), (uint8_t *)rgbBuffer.data(), buffSize);

            else if (_currentFourcc == "MJPG")
            {
                //ScopedMeasure (beginprevline + setpos(70) + "decomp ");
                _jpegDecompressor->decompressFrame((const uint8_t *)inBuffer.data(), buffSize, (uint8_t *)rgbBuffer.data(), _videoCapture->width, _videoCapture->height);
            }
            else
            {
                qd() << "error: unknown frame format: " << _currentFourcc;
                return;
            }
        }

        QImage img((const uint8_t*)rgbBuffer.data(), _videoCapture->width, _videoCapture->height, QImage::Format_RGB888);

        imageDispatch(img.copy()); // Копия наружу, правильно
    }

    emit finished();
}

// С пришедшим изображением можно делать все что угодно.
void Video4Private::imageDispatch(QImage img)
{
    const QString x = toReal3(db().value("xPos").toDouble());
    const QString y = toReal3(db().value("yPos").toDouble());

    img.setText("x", x);
    img.setText("y", y);
    //img.setDotsPerMeterX(db().pixInMm()*1000);
    //int resolutionWidth = db().value("resolution_width");

    qd() << "resol w " << db().pixInMm();
    img.setDevicePixelRatio(db().pixInMm());

    if (_capture || _captureSmallRegion)
    {
        if (_framesToThrowOut == 0)
        {
            int jpgFramesThrow = db().value("jpg_frames_throw").toInt();
            int yuvFramesThrow = db().value("yuv_frames_throw").toInt();

            _framesToThrowOut = _currentFourcc == "YUYV" ? yuvFramesThrow : jpgFramesThrow;

            if (_capture)
            {
                _capture = false;
                emit captured(img); // Копия наружу не нужна
            }

            if (_captureSmallRegion)
            {
                _captureSmallRegion = false;
                emit capturedSmallRegion(img.copy(_rectToCopy));
            }
        }
        else
        {
            _framesToThrowOut -= 1;
        }
    }

    emit rawImage(img); // Копию не делать
}

void Video4Private::changeCamera(int device, int width, int height, QString fourcc)
{
    _videoCapture.reset(new MyVideoDriver);
    _videoCapture->init(device, width, height, fourccToInt(fourcc));
    _currentFourcc = fourcc;
}
