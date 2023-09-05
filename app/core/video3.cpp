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

}

Video3::~Video3()
{
    Cap_closeStream(_ctx, _streamId);
    Cap_releaseContext(_ctx);
}

void Video3::init()
{
    qd() << Cap_getLibraryVersion();

    _ctx = Cap_createContext();
    qd() << "Context = " << _ctx;

    QStringList deviceNames;
    for(uint32_t device=0; device<Cap_getDeviceCount(_ctx); device++)
    {
        const QString devName = Cap_getDeviceName(_ctx, device);
        deviceNames.append(devName);

        for(int32_t format=0; format<Cap_getNumFormats(_ctx, device); format++)
        {
            CapFormatInfo finfo;
            Cap_getFormatInfo(_ctx, device, format, &finfo);

            const QString fourcc = fourccToString(finfo.fourcc);

            //QString formatName = QString::asprintf("%dx%d %s", finfo.width, finfo.height, fourcc.toLatin1().data());
            //_deviceInfo.push_back({device, format, deviceName, formatName});

            db().insert("camera" + device, QString("[%1x%2] %3 %4fps").arg(finfo.width, finfo.height).arg(fourcc).arg(finfo.fps));
        }
    }

    db().insert("cameras", deviceNames);
    //qd() << "cameras " << deviceNames;
}

void Video3::init2()
{
    CapDeviceID deviceID = 1;
    CapFormatID formatID = 3; //11

    _streamId = Cap_openStream(_ctx, deviceID, formatID);
    Cap_getFormatInfo(_ctx, deviceID, formatID, &_finfo);

    _frameData.resize(_finfo.width*_finfo.height*3);

    qd() << "Frame size:" << _finfo.width << _finfo.height;
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

void Video3::changeCamera(quint32 device, quint32 format)
{
    // kill currently running stream
    Cap_closeStream(_ctx, _streamId);

    qd() << "Opening new device/format: " << device << format;

    // open new stream
    _streamId = Cap_openStream(_ctx, device, format);

    qd() << "New stream ID" << _streamId;

    // resize the display window
    Cap_getFormatInfo(_ctx, device, format, &_finfo);
    _frameData.resize(_finfo.width*_finfo.height*3);

    emit newSize(_finfo.width, _finfo.height);
}

//std::vector<DeviceInfo> Video3::devicesInfo()
//{
//    return _deviceInfo;
//}
