#include "video3.h"
#include "utils.h"

#include "data_bus.h"

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
{
    qd() << Cap_getLibraryVersion();
    _ctx = Cap_createContext();
    qd() << "Context = " << _ctx;
}

Video3::~Video3()
{
    Cap_closeStream(_ctx, _streamId);
    Cap_releaseContext(_ctx);
}

void Video3::reloadDevices()
{
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

            //QString formatName = QString::asprintf("%dx%d %s", finfo.width, finfo.height, fourcc.toLatin1().data());
            //_deviceInfo.push_back({device, format, deviceName, formatName});

            const QString format = QString("[%1x%2] %3 %4fps").arg(finfo.width).arg(finfo.height).arg(fourcc).arg(finfo.fps);

            formats.append(format);
        }
        db().insert("camera" + QString::number(deviceId), formats);
    }

    db().insert("cameras", devices);
}

void Video3::update()
{
    //qd() << "video update";

    bool hasNewFrame = false;
    {
        // Когда есть новый фрейм, почему то время запроса очень долгое 60 мс примерно. Когда нет фрейма - 0,001 мс
        //ScopedMeasure mes("has new frame");
        hasNewFrame = Cap_hasNewFrame(_ctx, _streamId);
    }

    if (hasNewFrame)
    {
        //ScopedMeasure mes("capture and convert frame");
        Cap_captureFrame(_ctx, _streamId, &_frameData[0], _frameData.size());

        QImage img((const uint8_t*)&_frameData[0],
                _finfo.width,
                _finfo.height,
                //m_finfo.width*3,
                QImage::Format_RGB888);

        QByteArray imgPpm;// = writeBufferAsPPM(_finfo.width, _finfo.height, &_frameData[0], _frameData.size());

        if (img.isNull())
        {
            qd() << "Cannot create image..";
            return;
        }

        QString frameInfo = QString::asprintf("%d x %d frames:%d",
                                              _finfo.width,
                                              _finfo.height,
                                              Cap_getStreamFrameCount(_ctx, _streamId));


        //qd() << " new image ready";
        emit newImage(img, frameInfo, imgPpm);
    }
    else
    {
        //qd() << "no new frame";
    }
}

void Video3::changeCamera(quint32 deviceId, quint32 formatId)
{
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
