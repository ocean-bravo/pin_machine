#include "video3.h"
#include "utils.h"



Video3::Video3()
{

}

Video3::~Video3()
{
    Cap_closeStream(m_ctx, m_streamID);
    Cap_releaseContext(m_ctx);
}

void Video3::init()
{
    qd() << Cap_getLibraryVersion();

    m_ctx = Cap_createContext();
    qd() << "Context = " << m_ctx;

    CapDeviceID deviceID = 0;
    CapFormatID formatID = 0;

    m_streamID = Cap_openStream(m_ctx, deviceID, formatID);
    Cap_getFormatInfo(m_ctx, deviceID, formatID, &m_finfo);

    m_frameData.resize(m_finfo.width*m_finfo.height*3);

    qd() << "Frame size:" << m_finfo.width << m_finfo.height;

    for(uint32_t device=0; device<Cap_getDeviceCount(m_ctx); device++)
    {
        QString deviceName = Cap_getDeviceName(m_ctx, device);
        for(int32_t format=0; format<Cap_getNumFormats(m_ctx, device); format++)
        {
            CapFormatInfo finfo;
            Cap_getFormatInfo(m_ctx, device, format, &finfo);

            QString fourcc;
            for(uint32_t i=0; i<4; i++)
            {
                fourcc += (char)(finfo.fourcc & 0xFF);
                finfo.fourcc >>= 8;
            }

            QString formatName = QString::asprintf("%dx%d %s", finfo.width, finfo.height, fourcc.toLatin1().data());
            _deviceInfo.push_back({device, format, deviceName, formatName});
        }
    }
}

void Video3::update()
{
    if (Cap_hasNewFrame(m_ctx, m_streamID))
    {
        Cap_captureFrame(m_ctx, m_streamID, &m_frameData[0], m_frameData.size());
        QImage img((const uint8_t*)&m_frameData[0],
                m_finfo.width,
                m_finfo.height,
                //m_finfo.width*3,
                QImage::Format_RGB888);

        if (img.isNull())
        {
            qd() << "Cannot create image..";
            return;
        }

        QString frameInfo = QString::asprintf("%d x %d frames:%d",
                                              m_finfo.width,
                                              m_finfo.height,
                                              Cap_getStreamFrameCount(m_ctx, m_streamID));


        emit newImage(img, frameInfo);
    }
}

void Video3::changeCamera(quint32 device, quint32 format)
{
    // kill currently running stream
    Cap_closeStream(m_ctx, m_streamID);

    qd() << "Opening new device/format: " << device << format;

    // open new stream
    m_streamID = Cap_openStream(m_ctx, device, format);

    qd() << "New stream ID" << m_streamID;

    // resize the display window
    Cap_getFormatInfo(m_ctx, device, format, &m_finfo);
    m_frameData.resize(m_finfo.width*m_finfo.height*3);

    emit newSize(m_finfo.width, m_finfo.height);
}

std::vector<DeviceInfo> Video3::devicesInfo()
{
    return _deviceInfo;
}
