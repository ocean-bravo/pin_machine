#pragma once

#include <linux/videodev2.h>

#include <vector>
#include <QString>
#include <QRect>

class QJsonArray;

class MyDriver
{	
public:
    MyDriver();
    ~MyDriver();

    bool init(int device, int width, int height, int fourcc);
    bool isReady();
    bool start();
    bool stop();
    bool open(QString deviceName);
    void close();

    bool isReadable(int timeoutMs) const;

    size_t bufSize() const;

    size_t readInternal(char* buffer, size_t bufferSize);

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
