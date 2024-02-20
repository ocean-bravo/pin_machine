#pragma once

#include <linux/videodev2.h>

#include <vector>
#include <QString>
#include <QRect>

class QJsonArray;

class MyVideoDriver
{	
public:
    MyVideoDriver();
    ~MyVideoDriver();

    bool init(int device, int width, int height, int fourcc);
    bool isReady();
    bool start();
    bool stop();
    bool open(QString deviceName);
    void close();

    bool isReadable(int timeoutMs) const;

    size_t bufSize() const;

    size_t readInternal(char* buffer, size_t bufferSize);


    // V4L2_EXPOSURE_AUTO	Automatic exposure time, automatic iris aperture.
    // V4L2_EXPOSURE_MANUAL	Manual exposure time, manual iris.
    // V4L2_EXPOSURE_SHUTTER_PRIORITY	Manual exposure time, auto iris.
    // V4L2_EXPOSURE_APERTURE_PRIORITY	Auto exposure time, manual iris.
    void setExposureType(v4l2_exposure_auto_type type);
    void setExposureTime(qint32 value); // 1 unit - 100 us
    void setApertureSize(qint32 value);

    bool setProperty(const QString& property, qint32 value);

    int width = 640;
    int height = 480;

    static void reloadDevices();
    static quint32 maxFrameRate(int fd, quint32 pixelformat, quint32 width, quint32 height);
    static QVector<QRect> frameSizes(int fd, quint32 pixelformat);
    static QJsonArray imageFormats(int fd);

protected:
    size_t _bufSize = 0;

    unsigned int  n_buffers ;

    struct buffer
    {
        void* start = nullptr;
        size_t length = 0;
    };
    std::vector<buffer> m_buffers;

private:
    bool requestBuffers(int count);
    bool queryBuffers(int count);

    bool streamOn();
    bool streamOff();

    bool queueBuffers(int count);

    bool setFormat(int width, int height, int fourcc);

    bool freeBuffers();
    bool unmapAndDeleteBuffers();

    static v4l2_buf_type _type;
    QString _deviceName;
    int _fd = -1;
    static quint32 _memory;
};
