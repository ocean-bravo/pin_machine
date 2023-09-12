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
#include "V4l2Capture.h"

#include "yuvconverters.h"

namespace {

}


Video4::Video4()
{

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
    if (_thread)
    {
        delete _impl;
        _thread->quit();
        _thread->wait(5000);

    }

    _impl = new Video4Private;
    _thread = new QThread;

    connect(_impl, &Video4Private::newImage,   this, &Video4::newImage, Qt::QueuedConnection);


//    connect(_impl, &Video4Private::finished, _impl, &QObject::deleteLater);
//    connect(_impl, &Video4Private::finished, _thread, &QThread::quit);

    connect(_thread, &QThread::started,  _impl, &Video4Private::update);

    connect(_thread, &QThread::finished, _impl, &QObject::deleteLater);
    connect(_thread, &QThread::finished, _thread, &QObject::deleteLater);

    _impl->moveToThread(_thread);
    _thread->start();

    //QMetaObject::invokeMethod(_impl, "changeCamera", Qt::QueuedConnection, Q_ARG(quint32, deviceId), Q_ARG(quint32, formatId));
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

}

#include <QEventLoop>
#include <QTimer>

void Video4Private::update()
{
    changeCamera(0,0);

    while (true)
    {

        QEventLoop loop;
        QTimer::singleShot(10, &loop, &QEventLoop::quit);
        loop.exec();

        //ScopedMeasure ("update");

        if (_videoCapture.isNull())
            return;

        bool hasFrame;
        {
            //ScopedMeasure ("is readable ");
            hasFrame = _videoCapture->isReadable();
        }

        if (hasFrame)
        {
            char buffer[_videoCapture->getBufferSize()];
            int rsize = _videoCapture->read(buffer, sizeof(buffer));
            if (rsize == -1)
            {
                qd() << "stop " << strerror(errno);
            }
            else
            {
               // qd() << "size:" << rsize;

                char rgbBuffer[_videoCapture->getBufferSize()];

                YUYV2RGB((const uint8_t*)buffer, (uint8_t*)rgbBuffer, _videoCapture->getBufferSize());

                QImage img((const uint8_t*)rgbBuffer,
                           640,
                           480,
                           //m_finfo.width*3,
                           QImage::Format_RGB888);

                emit newImage(img, "", QByteArray());
            }
        }
    }
}

void Video4Private::changeCamera(quint32 deviceId, quint32 formatId)
{
    int verbose = 2;
    const char *in_devname = "/dev/video0";
    V4l2IoType ioTypeIn  = IOTYPE_MMAP; // Или IOTYPE_READWRITE
    int format = V4l2Device::fourcc("YUYV");
    int width = 640;
    int height = 480;
    int fps = 30;

    V4L2DeviceParameters param(in_devname, format, width, height, fps, ioTypeIn, verbose);
    _videoCapture.reset(V4l2Capture::create(param));

    if (_videoCapture.isNull())
    {
        qd() << "Cannot reading from V4L2 capture interface for device:" << in_devname;
        return;
    }

    //update();

}
