#include "V4l2MmapDevice.h"

#include "data_bus.h"
#include "common.h"

#include <QScopeGuard>
#include <QJsonArray>
#include <QJsonObject>
#include <QRect>
#include <sys/poll.h>

#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h> 
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sys/fcntl.h>

#include "utils.h"
// libv4l2


// project
//#include "logger.h"
#include "iostream"
#define LOG(a) std::cout

#include <unistd.h>

namespace {

const size_t V4L2MMAP_NBBUFFER = 2;

}

v4l2_buf_type MyDriver::_type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
quint32 MyDriver::_memory = V4L2_MEMORY_MMAP;

MyDriver::MyDriver()
    : n_buffers(V4L2MMAP_NBBUFFER)
{
    m_buffers.resize(V4L2MMAP_NBBUFFER);
    std::fill(m_buffers.begin(), m_buffers.end(), buffer());
}

MyDriver::~MyDriver()
{
    stop();
}

bool MyDriver::init(int device, int width, int height, int fourcc)
{
    qd() << "Camera: init device";

    bool ok = open(QString("/dev/video%1").arg(device));

    if (!ok)
        return false;

    this->width = width;
    this->height = height;
    setFormat(width, height, fourcc);

    start();
    return true;
}

bool MyDriver::open(QString deviceName)
{
    const QByteArray ba = deviceName.toLatin1();

    _deviceName = deviceName;
    _fd = ::open(ba.constData(), O_RDWR); // TODO: возможно NONBLOCK

    if (_fd < 0)
    {
        qd() <<  "Camera: error cannot open device" << deviceName << strerror(errno);
        return false;
    }
    qd() << "Camera: open device" << deviceName << "OK";
    return true;
}

void MyDriver::close()
{
    ::close(_fd);
    _fd = -1;
}

bool MyDriver::start()
{
    qd() << "Camera: starting device " << _deviceName << "...";

    if (!requestBuffers(V4L2MMAP_NBBUFFER))
        return false;

    if (!queryBuffers(V4L2MMAP_NBBUFFER))
        return false;

    if (!streamOn())
        return false;

    if (!queueBuffers(V4L2MMAP_NBBUFFER))
        return false;

    qd() << "Camera: start device OK";
    return true;
}

bool MyDriver::stop()
{
    qd() << "Camera: stop device " << _deviceName;

    streamOff();
    unmapAndDeleteBuffers();
    freeBuffers();

    return true;
}

bool MyDriver::isReady()
{
    return  ((_fd != -1)&& (n_buffers != 0));
}

bool MyDriver::requestBuffers(int count)
{
    v4l2_requestbuffers req = {};

    req.count   = count;
    req.type    = _type;
    req.memory  = _memory;

    if (ioctl(_fd, VIDIOC_REQBUFS, &req) == -1) { qd() << "Camera: error VIDIOC_REQBUFS"; return false; }
    qd() << "Camera: request buffers OK"; return true;
}

bool MyDriver::queryBuffers(int count)
{
    for (int i = 0; i < count; ++i)
    {
        v4l2_buffer buf = {};

        buf.type   = _type;
        buf.memory = _memory;
        buf.index  = i;

        if (ioctl(_fd, VIDIOC_QUERYBUF, &buf) == -1)
        {
            qd() << "Camera: error VIDIOC_QUERYBUF " << i;
            return false;
        }
        auto bufff = mmap (NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, _fd, buf.m.offset);

        if (bufff == MAP_FAILED)
        {
            qd() << "Camera: error mmap " << i;
            return false;
        }

        // какая то проверка buf.bytesused могла бы быть

        m_buffers[i].start = bufff;
        m_buffers[i].length = buf.length;
        _bufSize = buf.length;
    }
    qd() << QString("Camera: query %1 buffers OK (size %2)").arg(count).arg(_bufSize);
    return true;
}

bool MyDriver::queueBuffers(int count)
{
    for (int i = 0; i < count; ++i)
    {
        v4l2_buffer buf = {};

        buf.type   = _type;
        buf.memory = _memory;
        buf.index  = i;

        if (ioctl(_fd, VIDIOC_QBUF, &buf) == -1) { qd() << "Camera: error VIDIOC_QBUF " << i; return false; }
    }
    return true;
}

bool MyDriver::setFormat(int width, int height, int fourcc)
{
    v4l2_format format = {};
    format.type                = _type;
    format.fmt.pix.width       = width;
    format.fmt.pix.height      = height;
    format.fmt.pix.pixelformat = fourcc;
    format.fmt.pix.field       = V4L2_FIELD_NONE;

    if (ioctl(_fd, VIDIOC_S_FMT, &format) == -1)
    {
        qd() << QString("Camera: error could not set format (%1x%2 %3)").arg(width).arg(height).arg(fourccToString(fourcc));
        return false;
    }

    // Можно еще перечитать структуру для большей уверенности, что формат установился.

    qd() << QString("Camera: set format (%1x%2 %3) OK").arg(width).arg(height).arg(fourccToString(fourcc));
    return true;
}

bool MyDriver::unmapAndDeleteBuffers()
{
    bool result = true;
    for(uint32_t i=0; i< m_buffers.size(); i++)
    {
        if (munmap(m_buffers[i].start, m_buffers[i].length) == -1)
        {
            qd() << "Camera: error munmap";
            result = false;
        }
    }

    m_buffers.clear();

    qd() << "Camera: munmap and delete buffers OK";
    return result;
}

bool MyDriver::freeBuffers()
{
    n_buffers = 0;

    v4l2_requestbuffers req = {};

    req.count    = 0;
    req.type     = _type;
    req.memory   = _memory;

    if (ioctl(_fd, VIDIOC_REQBUFS, &req) == -1) { qd() << "Camera: error VIDIOC_REQBUFS"; return false; }
    qd() << "Camera: free buffers OK";
    return true;
}

bool MyDriver::isReadable(int timeoutMs) const
{
    if (_fd == -1)
    {
        qd() << "Camera: error fd = -1";
        return false;
    }
    // Use poll to wait for video capture events
    pollfd poll_fds[1];
    poll_fds[0].fd = _fd;
    poll_fds[0].events = POLLIN;
    const int ret = poll(poll_fds, 1, timeoutMs); // wait up to 1 second for events ( put -1 if it should wait forever )
    return ret == 1;
}

size_t MyDriver::bufSize() const
{
    return _bufSize;
}

bool MyDriver::streamOn()
{
    if (ioctl(_fd, VIDIOC_STREAMON, &_type) == -1) { qd() << "Camera: error VIDIOC_STREAMON"; return false; }
    qd() << "Camera: stream on OK"; return true;
}

bool MyDriver::streamOff()
{
    if (ioctl(_fd, VIDIOC_STREAMOFF, &_type) == -1) { qd() << "Camera: error VIDIOC_STREAMOFF"; return false; }
    qd() << "Camera: stream off OK"; return true;
}

//void grab_yuyvframe() {
//    queue_buffer(camera);
//    //Wait for io operation
//    fd_set fds;
//    FD_ZERO(&fds);
//    FD_SET(camera, &fds);
//    struct timeval tv = {0};
//    tv.tv_sec = 2; //set timeout to 2 second
//    int r = select(camera+1, &fds, NULL, NULL, &tv);
//    if(-1 == r){
//        perror("Waiting for Frame");
//        exit(1);
//    }
//    int file = open("output.yuy", O_WRONLY);
//    write(file, buffer, size); //size is obtained from the query_buffer function
//    dequeue_buffer(camera);
//}

size_t MyDriver::readInternal(char* buffer, size_t bufferSize)
{
    size_t size = 0;

    if (n_buffers > 0)
    {
        v4l2_buffer buf = {};
        buf.type   = _type;
        buf.memory = _memory;

        if (ioctl(_fd, VIDIOC_DQBUF, &buf) == -1)
        {
            qd() << "Camera: error VIDIOC_DQBUF";
            size = -1;
        }
        else if (buf.index < n_buffers)
        {
            size = buf.bytesused;
            if (size > bufferSize)
            {
                size = bufferSize;
                qd() << " warn device " << _deviceName << " buffer truncated available:" << bufferSize << " needed:" << buf.bytesused;
            }
            memcpy(buffer, m_buffers[buf.index].start, size);

            if (ioctl(_fd, VIDIOC_QBUF, &buf) == -1)
            {
                qd() <<  "Camera: error VIDIOC_QBUF";
                size = -1;
            }
        }
    }
    return size;
}

quint32 MyDriver::maxFrameRate(int fd, quint32 pixelformat, quint32 width, quint32 height)
{
    quint32 fps = 0;

    v4l2_frmivalenum frmival = {};

    frmival.pixel_format = pixelformat;
    frmival.width = width;
    frmival.height = height;
    frmival.index = 0;

    while (ioctl(fd, VIDIOC_ENUM_FRAMEINTERVALS, &frmival) != -1)
    {
        if (frmival.type == V4L2_FRMIVAL_TYPE_DISCRETE)
        {
            quint32 v = frmival.discrete.denominator / frmival.discrete.numerator;

            if (fps < v)
                fps = v;
        }
        ++frmival.index;
    }

    return fps;
}

QJsonArray MyDriver::imageFormats(int fd)
{
    v4l2_fmtdesc fmtdesc = {};

    fmtdesc.index = 0;
    fmtdesc.type = _type;

    QJsonArray formats;

    while (ioctl(fd, VIDIOC_ENUM_FMT, &fmtdesc) != -1)      // Получите формат, поддерживаемый выходным изображением камеры.
    {
        const QString fourcc = fourccToString(fmtdesc.pixelformat);

        ++fmtdesc.index;

        const QVector<QRect> sizes = frameSizes(fd, fmtdesc.pixelformat);

        for (const QRect& size : sizes)
        {
            const quint32 fps = maxFrameRate(fd, fmtdesc.pixelformat, size.width(), size.height());

            const QString display = QString("[%1x%2] %3 %4fps").arg(size.width()).arg(size.height()).arg(fourcc).arg(fps);

            QJsonObject format {
                {"width",   int(size.width())},
                {"height",  int(size.height())},
                {"fourcc",  fourcc},
                {"fps",     int(fps)},
                {"display", display}
            };

            formats.append(format);
        }
    }
    return formats;
}

QVector<QRect> MyDriver::frameSizes(int fd, quint32 pixelformat)
{
    v4l2_frmsizeenum frmsize = {};

    frmsize.index = 0;
    frmsize.pixel_format = pixelformat;

    QVector<QRect> frameSizes;

    while (ioctl(fd, VIDIOC_ENUM_FRAMESIZES, &frmsize) != -1)
    {
        QRect frameSize;
        if (frmsize.type == V4L2_FRMSIZE_TYPE_DISCRETE)
        {
            frameSize.setWidth(frmsize.discrete.width);
            frameSize.setHeight(frmsize.discrete.height);
            frameSizes.push_back(frameSize);
        }

        ++frmsize.index;
    }

    return frameSizes;
}

void MyDriver::reloadDevices()
{
    const int maxDevices = 32;

    QJsonArray devices;

    for (int id = 0; id < maxDevices; ++id)
    {
        const QByteArray path = QString("/dev/video%1").arg(id).toLatin1();

        const int fd = ::open(path.constData(), O_RDWR);
        auto fileCloser = qScopeGuard([=]() { ::close(fd); });

        if (fd == -1)
            continue;

        v4l2_capability cap = {};
        if (ioctl(fd, VIDIOC_QUERYCAP, &cap) == -1)        // Проверьте информацию о камере
        {
            qd() << "Camera: cannot get capabilities for device: " << path << " " << strerror(errno);
            continue;
        }
        else
        {
            qd() << "path:\t\t"           << path;
            qd() << "driver_name:\t\t"    << QString::fromLatin1(reinterpret_cast<const char *>(cap.driver));
            qd() << "device_name:\t\t"    << QString::fromUtf8(reinterpret_cast<const char *>(cap.card));
            qd() << "bus_info:\t\t"       << QString::fromLatin1(reinterpret_cast<const char *>(cap.bus_info));
            qd() << "driver_version:\t"   << QString("%1.%2.%3").arg((cap.version >> 16) & 0xFF).arg((cap.version >> 8) & 0xFF).arg((cap.version) & 0XFF),
            qd() << "capabilities:\t\t"   << QByteArray().setNum(cap.capabilities, 16); // набор возможностей, обычно：V4L2_CAP_VIDEO_CAPTURE | V4L2_CAP_STREAMING
            qd() << "device_caps:\t\t"    << QByteArray().setNum(cap.device_caps, 16);
        }

        const QJsonArray formats = imageFormats(fd);

        // Если у устройства нет форматов, не заносить его в список устройств.
        if (formats.isEmpty())
            continue;

        db().insert("camera" + QString::number(id), formats);

        QJsonObject device {
            { "id",   id },
            { "name", QString::fromUtf8(reinterpret_cast<const char *>(cap.card)) }
        };

        devices.append(device);
    }
    db().insert("cameras", devices);
}
