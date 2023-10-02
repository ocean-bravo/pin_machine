#include "video4.h"
#include "utils.h"

#include "data_bus.h"

#include <QBuffer>
#include <QEventLoop>
#include <QTimer>
#include <QDateTime>

#include <linux/videodev2.h>

#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <signal.h>

//#include "logger.h"
//#include "V4l2Capture.h"
#include "V4l2MmapDevice.h"
#include "yuvconverters.h"

#include "mjpeghelper.h"

namespace {

const int throwFramesYuv = 1;
const int throwFramesJpg = 4;
}


Video4::Video4()
{
    _impl = new Video4Private;
    _thread = new QThread;

    connect(_impl, &Video4Private::newImage,   this, &Video4::newImage, Qt::QueuedConnection);
    connect(_impl, &Video4Private::captured,   this, &Video4::captured, Qt::QueuedConnection);

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

void Video4::start()
{
    QMetaObject::invokeMethod(_impl, "start", Qt::QueuedConnection);
}

void Video4::stop()
{
    QMetaObject::invokeMethod(_impl, "stop", Qt::QueuedConnection);
}

void Video4::capture()
{
    QMetaObject::invokeMethod(_impl, "capture", Qt::QueuedConnection);
}

void Video4Private::reloadDevices()
{

}

void Video4Private::init()
{
//    if (_running)
//    {
//        QEventLoop loop;
//        QTimer::singleShot(1000, &loop, &QEventLoop::quit);
//        QObject::connect(this, &Video4Private::stopped, &loop, &QEventLoop::quit);
//        _running = false;
//        loop.exec();
//    }

    _jpegDecompressor = new MjpegHelper;
}

void Video4Private::start()
{
    _running = true;
    QMetaObject::invokeMethod(this, "update", Qt::QueuedConnection);
}

void Video4Private::stop()
{
    _running = false;
}

void Video4Private::capture()
{
    qd() << "Video4Private::capture";
    _capture = true;
    _framesToThrowOut = _currentFourcc == "YUYV" ? throwFramesYuv : throwFramesJpg;
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

    QEventLoop loop;

    while (true)
    {
        //qd() << QDateTime::currentMSecsSinceEpoch();

        //ScopedMeasure ("updat");
        QTimer::singleShot(1, &loop, &QEventLoop::quit);
        loop.exec();

        if (!_running)
            break;

        if (!_videoCapture)
            break;

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
        //qd() << i << ":" << 1000/elapsed.count() << "fps";
        start = std::chrono::steady_clock::now();

        {
            //ScopedMeasure ("YUYV2RGB");

            if (_currentFourcc == "YUYV")
                YUYV2RGB((const uint8_t *)inBuffer.data(), (uint8_t *)rgbBuffer.data(), buffSize);

            else if (_currentFourcc == "MJPG")
            {
                _jpegDecompressor->decompressFrame((const uint8_t *)inBuffer.data(), buffSize, (uint8_t *)rgbBuffer.data(), _videoCapture->width, _videoCapture->height);
            }
            else
            {
                qd() << "error: unknown frame format: " << _currentFourcc;
                return;
            }
        }
        {
            //ScopedMeasure ("QImage ");
            QImage img((const uint8_t*)rgbBuffer.data(), _videoCapture->width, _videoCapture->height, QImage::Format_RGB888);
            emit newImage(img.copy()); // Наружу выпускается копия, все правильно
            //qd() << "\n";

            if (_capture)
            {
                if (_framesToThrowOut == 0)
                {
                    _framesToThrowOut = _currentFourcc == "YUYV" ? throwFramesYuv : throwFramesJpg;
                    _capture = false;
                    qd() << "Video4Private::update captured";

                    emit captured(img.copy()); // Наружу выпускается копия, все правильно
                }
                else
                {
                    _framesToThrowOut -= 1;
                }
            }
        }
    }

    emit stopped();
}

void Video4Private::changeCamera(int device, int width, int height, QString fourcc)
{
    _videoCapture.reset(new V4l2MmapDevice);
    _videoCapture->init(device, width, height, V4l2Device::fourcc(fourcc.toLatin1().toStdString().c_str()));
    _currentFourcc = fourcc;
}
