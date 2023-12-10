#include "video4.h"
#include "utils.h"
#include "wait.h"

#include "data_bus.h"

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

#include "V4l2MmapDevice.h"
#include "yuvconverters.h"

#include "mjpeghelper.h"

namespace {

// Сколько кадров нужно выкинуть.
const int throwFramesYuv = 1; // Достаточно 1, чтобы не было смаза. Не всегда...
const int throwFramesJpg = 15; // 12 вроде достаточно было

}


Video4::Video4()
{
    _impl = new Video4Private;
    _thread = new QThread;

    connect(_impl, &Video4Private::newImage,   this, &Video4::newImage, Qt::QueuedConnection);
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

void Video4::capture()
{
    QMetaObject::invokeMethod(_impl, "capture", Qt::QueuedConnection);
}

void Video4::captureSmallRegion(double width)
{
    QMetaObject::invokeMethod(_impl, "captureSmallRegion", Qt::QueuedConnection, Q_ARG(double, width));
}

QImage Video4::smallRegion()
{
    const double x = db().value("xPos").toDouble();
    const double y = db().value("yPos").toDouble();

    _smallRegion.setText("x", toReal3(x));
    _smallRegion.setText("y", toReal3(y));
    return _smallRegion;
}

void Video4::reloadDevices()
{
    QMetaObject::invokeMethod(_impl, "reloadDevices", Qt::QueuedConnection);
}


void Video4Private::init()
{
    _jpegDecompressor = new MjpegHelper;
    //connect(this, &Video4Private::imageCaptured, this, &Video4Private::imageDispatch, Qt::QueuedConnection);

    db().insert("jpg_frames_throw", throwFramesJpg);
    db().insert("yuv_frames_throw", throwFramesYuv);
}

void Video4Private::capture()
{
    //qd() << "Video4Private::capture";
    _capture = true;

    int jpgFramesThrow = db().value("jpg_frames_throw").toInt();
    int yuvFramesThrow = db().value("yuv_frames_throw").toInt();

    _framesToThrowOut = _currentFourcc == "YUYV" ? yuvFramesThrow  : jpgFramesThrow;
}

void Video4Private::captureSmallRegion(double width)
{
    _captureSmallRegion = true;
    _framesToThrowOut = _currentFourcc == "YUYV" ? throwFramesYuv : throwFramesJpg;
    _smallRegionWidthMm = width;
}

void Video4Private::reloadDevices()
{
    _videoCapture->reloadDevices();
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
        imageDispatch(img);
    }

    emit finished();
}

void Video4Private::imageDispatch(QImage img)
{
    //qd() << beginprevline + setpos(30) + "dispatch ...";
    //qd() << "dispatch ...";

    const QString x = db().value("x_coord").toString();
    const QString y = db().value("y_coord").toString();

    img.setText("x", x);
    img.setText("y", y);

    emit newImage(img.copy());

    if (_capture || _captureSmallRegion)
    {
        if (_framesToThrowOut == 0)
        {
            _framesToThrowOut = _currentFourcc == "YUYV" ? throwFramesYuv : throwFramesJpg;

            //qd() << "Video4Private::update captured";
            if (_capture)
            {
                _capture = false;

                static quint32 count = 0;
                ++count;
                //qd() << beginprevline << setpos(30) << "captured " << count;
                emit captured(img.copy()); // Наружу выпускается копия, все правильно
            }

            if (_captureSmallRegion)
            {
                int xCenter = img.width()/2;
                int yCenter = img.height()/2;
                _captureSmallRegion = false;

                int widthPix = _smallRegionWidthMm * db().pixInMm();

                emit capturedSmallRegion(img.copy(QRect(xCenter - (widthPix/2), yCenter - (widthPix/2), widthPix, widthPix)));
                //qd() << "small region captured";
            }
        }
        else
        {
            _framesToThrowOut -= 1;
        }
    }
}

void Video4Private::changeCamera(int device, int width, int height, QString fourcc)
{
    _videoCapture.reset(new MyDriver);
    _videoCapture->init(device, width, height, fourccToInt(fourcc));
    _currentFourcc = fourcc;
}
