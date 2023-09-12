#include "video3.h"
#include "utils.h"

#include "data_bus.h"

#include <QBuffer>

namespace {

QByteArray writeBufferAsPPM(uint32_t width, uint32_t height, const uint8_t *bufferPtr, size_t bytes)
{
    QByteArray pgmImage;
    {
        QTextStream textStream(&pgmImage);
        textStream << "P6" <<  ' ' << width << ' ' << height << ' ' <<  255 << '\n'; // PGM header
    }
    {
        QDataStream dataStream(&pgmImage, QIODevice::WriteOnly | QIODevice::Append);
        dataStream.writeRawData((const char *)bufferPtr, bytes);
    }

    return pgmImage;
}

QString fourccToString (quint32 fourcc)
{
    QString str;
    for(uint32_t i=0; i<4; i++)
    {
        str += (char)(fourcc & 0xFF);
        fourcc >>= 8;
    }
    return str;
}

}


Video3::Video3()
    : _impl(new Video3Private)
    , _thread(new QThread)
{
    connect(_impl, &Video3Private::newImage,   this, &Video3::newImage, Qt::QueuedConnection);
//    connect(this, &Logger::common, _impl, &LoggerPrivate::common, Qt::QueuedConnection);

//    connect(_impl, &LoggerPrivate::logToFileChanged, this, &Logger::logToFileChanged, Qt::QueuedConnection);
//    connect(_impl, &LoggerPrivate::inited,           this, &Logger::inited, Qt::QueuedConnection);

    connect(_thread.data(), &QThread::finished, _impl, &QObject::deleteLater);
    connect(_thread.data(), &QThread::started,  _impl, &Video3Private::init, Qt::QueuedConnection);

    _impl->moveToThread(_thread.data());
    _thread->start();
}

Video3::~Video3()
{
    _thread->quit();
    _thread->wait(1000);
}

void Video3::reloadDevices()
{
    QMetaObject::invokeMethod(_impl, "reloadDevices", Qt::QueuedConnection);
}

void Video3::changeCamera(quint32 deviceId, quint32 formatId)
{
    QMetaObject::invokeMethod(_impl, "changeCamera", Qt::QueuedConnection, Q_ARG(quint32, deviceId), Q_ARG(quint32, formatId));
}

void Video3::update()
{
    QMetaObject::invokeMethod(_impl, "update", Qt::QueuedConnection);
}


Video3Private::Video3Private()
{

}

Video3Private::~Video3Private()
{
    if (_ctx != nullptr && _streamId > -1)
        Cap_closeStream(_ctx, _streamId);

    if (_ctx != nullptr)
        Cap_releaseContext(_ctx);
}

void Video3Private::reloadDevices()
{
    if (_ctx != nullptr && _streamId > -1)
        Cap_closeStream(_ctx, _streamId);

    if (_ctx != nullptr)
        Cap_releaseContext(_ctx);

    _ctx = Cap_createContext();
    qd() << "Context = " << _ctx;

    QStringList devices;
    for(uint32_t deviceId = 0; deviceId < Cap_getDeviceCount(_ctx); deviceId++)
    {
        const QString devName = Cap_getDeviceName(_ctx, deviceId);
        devices.append(devName);

        QStringList formats;
        for(int32_t formatId = 0; formatId < Cap_getNumFormats(_ctx, deviceId); formatId++)
        {
            CapFormatInfo finfo;
            Cap_getFormatInfo(_ctx, deviceId, formatId, &finfo);

            const QString fourcc = fourccToString(finfo.fourcc);

            const QString format = QString("[%1x%2] %3 %4fps").arg(finfo.width).arg(finfo.height).arg(fourcc).arg(finfo.fps);

            formats.append(format);
        }
        db().insert("camera" + QString::number(deviceId), formats);
    }

    db().insert("cameras", devices);
}

void Video3Private::init()
{
    qd() << Cap_getLibraryVersion();

    reloadDevices();
}

void Video3Private::update()
{
    //qd() << "video update";

    if (_streamId == -1)
        return;

    bool hasNewFrame = false;
    {
        // Когда есть новый фрейм, почему то время запроса очень долгое 60 мс примерно. Когда нет фрейма - 0,001 мс
        //ScopedMeasure mes("has new frame");
        hasNewFrame = Cap_hasNewFrame(_ctx, _streamId);
    }

    if (hasNewFrame)
    {

        //std::fill(_frameData.begin(), _frameData.end(), 0);
        {
            ScopedMeasure mes("capture frame");
            Cap_captureFrame(_ctx, _streamId, &_frameData[0], _frameData.size());

        }
        qd() << "\n";



        QImage imag((const uint8_t*)&_frameData[0],
                _finfo.width,
                _finfo.height,
                //m_finfo.width*3,
                QImage::Format_RGB888);

        QImage img = imag.copy();


        //QByteArray imgPpm;// = writeBufferAsPPM(_finfo.width, _finfo.height, &_frameData[0], _frameData.size());

        QByteArray ba;
        //QBuffer buffer(&ba);
        //buffer.open(QIODevice::WriteOnly);
        //buffer.setData((const char *)_frameData.data(), _frameData.size());
        //buffer.close();

        if (img.isNull())
        {
            qd() << "Cannot create image..";
            return;
        }

//        QString frameInfo = QString::asprintf("%d x %d frames:%d",
//                                              _finfo.width,
//                                              _finfo.height,
//                                              Cap_getStreamFrameCount(_ctx, _streamId));


        //qd() << " stream frame count " << frameInfo;
        emit newImage(img, "", ba);
    }
    else
    {
        //qd() << "no new frame";
    }
}

void Video3Private::changeCamera(quint32 deviceId, quint32 formatId)
{
    if (_streamId != -1)
        Cap_closeStream(_ctx, _streamId); // kill currently running stream

    qd() << "Opening new device/format: " << deviceId << formatId;

    // open new stream
    _streamId = Cap_openStream(_ctx, deviceId, formatId);

    qd() << "New stream ID" << _streamId;

    // resize the display window
    Cap_getFormatInfo(_ctx, deviceId, formatId, &_finfo);
    _frameData.resize(_finfo.width*_finfo.height*3);
    qd() << "Frame size:" << _finfo.width << _finfo.height;
    emit newSize(_finfo.width, _finfo.height);
}

//std::vector<DeviceInfo> Video3::devicesInfo()
//{
//    return _deviceInfo;
//}
