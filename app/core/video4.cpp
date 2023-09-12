#include "video4.h"
#include "utils.h"

#include "data_bus.h"

#include <QBuffer>

#include <linux/videodev2.h>

#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <signal.h>

//#include "logger.h"
//#include "V4l2Capture.h"
#include "V4l2MmapDevice.h"

#include "yuvconverters.h"

#include <QEventLoop>
#include <QTimer>

namespace {

}


Video4::Video4()
{

    _impl = new Video4Private;
    _thread = new QThread;

    connect(_impl, &Video4Private::newImage,   this, &Video4::newImage, Qt::QueuedConnection);


    //    connect(_impl, &Video4Private::finished, _impl, &QObject::deleteLater);
    //    connect(_impl, &Video4Private::finished, _thread, &QThread::quit);

    connect(_thread, &QThread::started,  _impl, &Video4Private::init);

    connect(_thread, &QThread::finished, _impl, &QObject::deleteLater);
    connect(_thread, &QThread::finished, _thread, &QObject::deleteLater);

    _impl->moveToThread(_thread);
    _thread->start();



    //    connect(this, &Logger::common, _impl, &LoggerPrivate::common, Qt::QueuedConnection);

    //    connect(_impl, &LoggerPrivate::logToFileChanged, this, &Logger::logToFileChanged, Qt::QueuedConnection);
    //    connect(_impl, &LoggerPrivate::inited,           this, &Logger::inited, Qt::QueuedConnection);

    //changeCamera(0, 0);
}

Video4::~Video4()
{
    _thread->quit();
    _thread->wait(1000);
}

void Video4::reloadDevices()
{
    //   QMetaObject::invokeMethod(_impl.data(), "reloadDevices", Qt::QueuedConnection);
}

void Video4::changeCamera(quint32 deviceId, quint32 formatId)
{
    QMetaObject::invokeMethod(_impl, "changeCamera", Qt::QueuedConnection, Q_ARG(quint32, deviceId), Q_ARG(quint32, formatId));
}

//void Video4::update()
//{
//    QMetaObject::invokeMethod(_impl, "update", Qt::QueuedConnection);
//}


Video4Private::Video4Private()
{
    qd() << "constructor";
}

Video4Private::~Video4Private()
{
    qd() << "destructor";
}

void Video4Private::reloadDevices()
{
}

void Video4Private::init()
{
    if (_running)
    {
        QEventLoop loop;
        QTimer::singleShot(1000, &loop, &QEventLoop::quit);
        QObject::connect(this, &Video4Private::stopped, &loop, &QEventLoop::quit);
        _running = false;
        loop.exec();
    }

    changeCamera(0,0);

    _running = true;

    QMetaObject::invokeMethod(this, "update", Qt::QueuedConnection);
}



void Video4Private::update()
{
    while (true)
    {
        //ScopedMeasure ("update");

        if (!_running)
            break;

        _videoCapture->isReady();

        if (!_videoCapture->isReady())
            break;

        const bool hasFrame = _videoCapture->isReadable(100);

        if (!hasFrame)
            continue;

        const quint32 buffSize = _videoCapture->bufSize();

         //qd() << "buffSize:" << buffSize;


        char inBuffer[buffSize];

        int rsize = _videoCapture->readInternal(inBuffer, buffSize);
        if (rsize == -1)
        {
            qd() << "stop " << strerror(errno);
            break;
        }

         qd() << "size:" << rsize;

        char rgbBuffer[buffSize];

        YUYV2RGB((const uint8_t *)inBuffer, (uint8_t *)rgbBuffer, buffSize);

        QImage img((const uint8_t*)rgbBuffer, 640, 480, QImage::Format_RGB888);

        emit newImage(img, "", QByteArray());

        QEventLoop loop;
        QTimer::singleShot(10, &loop, &QEventLoop::quit);
        loop.exec();
    }

    emit stopped();
}

void Video4Private::changeCamera(quint32 deviceId, quint32 formatId)
{
    int verbose = 2;
    const char* in_devname = "/dev/video0";
    V4l2IoType ioTypeIn  = IOTYPE_MMAP; // Или IOTYPE_READWRITE
    int format = V4l2Device::fourcc("YUYV");
    int width = 640;
    int height = 480;
    int fps = 30;

    //V4L2DeviceParameters param(in_devname, format, width, height, fps, ioTypeIn, verbose);
    if (_videoCapture)
        delete _videoCapture;

    _videoCapture = new V4l2MmapDevice;
    _videoCapture->init();

//    int caps = V4L2_CAP_VIDEO_CAPTURE | V4L2_CAP_STREAMING;


//    if (!_videoCapture->device())
//    {
//        qd() << "Cannot reading from V4L2 capture interface for device:" << in_devname;
//        return;
//    }
}
