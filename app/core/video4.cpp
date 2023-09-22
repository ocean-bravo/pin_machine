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

#include <QDateTime>

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

void Video4::changeCamera(int device, int width, int height, QString fourcc)
{
    QMetaObject::invokeMethod(_impl, "changeCamera", Qt::QueuedConnection, Q_ARG(int, device),Q_ARG(int, width),Q_ARG(int, height),Q_ARG(QString, fourcc));
}

void Video4::update()
{
    QMetaObject::invokeMethod(_impl, "update", Qt::QueuedConnection);
}


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

    //changeCamera(0,320,240, "YUYV");

    _running = true;

    QMetaObject::invokeMethod(this, "update", Qt::QueuedConnection);
}



void Video4Private::update()
{
    std::vector<char> inBuffer;
    inBuffer.reserve(20000000); // должно хватить

    std::vector<char> rgbBuffer;
    rgbBuffer.reserve(20000000); // должно хватить

    auto start = std::chrono::steady_clock::now();
    auto finish = std::chrono::steady_clock::now();

    int i = 0;
    while (true)
    {
        //qd() << QDateTime::currentMSecsSinceEpoch();

        //ScopedMeasure ("updat");

//        QEventLoop loop;
//        QTimer::singleShot(3, &loop, &QEventLoop::quit);
//        loop.exec();

        if (!_running)
            break;

        if (!_videoCapture)
            break;

        //_videoCapture->isReady();

        if (!_videoCapture->isReady())
            break;


        const bool hasFrame = _videoCapture->isReadable(10);

        if (!hasFrame)
            continue;

        const quint32 buffSize = _videoCapture->bufSize();

         //qd() << "buffSize:" << buffSize;
        {
            //ScopedMeasure ("resize");
            inBuffer.resize(buffSize);
            rgbBuffer.resize(buffSize);
        }
        {
            //ScopedMeasure ("read internal");
            int rsize = _videoCapture->readInternal(inBuffer.data(), buffSize);
            if (rsize == -1)
            {
                qd() << "stop " << strerror(errno);
                break;
            }
        }


        //qd() << "size:" << rsize;

        finish = std::chrono::steady_clock::now();
        const std::chrono::duration<double, std::milli> elapsed = finish - start;
        ++i;
        //qd() << i << ":" << elapsed.count() << "ms";
        qd() << i << ":" << 1000/elapsed.count() << "fps";
        start = std::chrono::steady_clock::now();

        {
            //ScopedMeasure ("YUYV2RGB");
            YUYV2RGB((const uint8_t *)inBuffer.data(), (uint8_t *)rgbBuffer.data(), buffSize);
        }
        {
            //ScopedMeasure ("QImage ");
            QImage img((const uint8_t*)rgbBuffer.data(), _videoCapture->width, _videoCapture->height, QImage::Format_RGB888);
            emit newImage(img.copy(), "", QByteArray());
            //qd() << "\n";
        }

    }

    emit stopped();
}

void Video4Private::changeCamera(int device, int width, int height, QString fourcc)
{
//    int verbose = 2;
//    const char* in_devname = "/dev/video0";
//    V4l2IoType ioTypeIn  = IOTYPE_MMAP; // Или IOTYPE_READWRITE
//    int format = V4l2Device::fourcc("YUYV");
//    int width = 640;
//    int height = 480;
//    int fps = 30;

    //V4L2DeviceParameters param(in_devname, format, width, height, fps, ioTypeIn, verbose);

    if (_videoCapture)
        delete _videoCapture;

    _videoCapture = new V4l2MmapDevice;
    _videoCapture->init(device, width, height, V4l2Device::fourcc(fourcc.toLatin1().toStdString().c_str()));

//    int caps = V4L2_CAP_VIDEO_CAPTURE | V4L2_CAP_STREAMING;


//    if (!_videoCapture->device())
//    {
//        qd() << "Cannot reading from V4L2 capture interface for device:" << in_devname;
//        return;
//    }
}
