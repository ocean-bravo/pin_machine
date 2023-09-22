#pragma once

#include <vector>
#include <QString>

#include "V4l2Device.h"

class V4l2MmapDevice
{	
public:
    V4l2MmapDevice();
    ~V4l2MmapDevice();

    //virtual bool init(unsigned int mandatoryCapabilities);
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

protected:


    size_t _bufSize = 0;


    unsigned int  n_buffers ;

    struct buffer
    {
        void* start = nullptr;
        size_t length = 0;
    };
    std::vector<buffer> m_buffers;

    bool unmapAndDeleteBuffers();

private:
    bool streamOff(int fd);
    bool requestBuffers(int fd, int count);
    bool queryBuffers(int fd, int count);
    bool streamOn(int fd);
    bool queueBuffers(int fd, int count);
    int setFormat(int fd, int width, int height, int fourcc);



    bool freeBuffers();



    QString _deviceName;

    int m_fd = -1;
};

