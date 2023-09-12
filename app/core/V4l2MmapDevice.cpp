#include "V4l2MmapDevice.h"

#include <sys/poll.h>

#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h> 
#include <sys/mman.h>
#include <sys/ioctl.h>

#include "utils.h"
// libv4l2
#include <linux/videodev2.h>

// project
//#include "logger.h"
#include "iostream"
#define LOG(a) std::cout

#include <unistd.h>

namespace {

const size_t V4L2MMAP_NBBUFFER = 2;

}

V4l2MmapDevice::V4l2MmapDevice()
    : n_buffers(V4L2MMAP_NBBUFFER)
{
    m_buffers.resize(V4L2MMAP_NBBUFFER);
    std::fill(m_buffers.begin(), m_buffers.end(), buffer());
}

//bool V4l2MmapDevice::init(unsigned int mandatoryCapabilities)
//{
//    qd() << "init device";

//    bool ret = V4l2Device::init(mandatoryCapabilities);
//    if (ret)
//    {
//        qd() << "inited OK";
//        ret = start();
//    }
//    return ret;
//}

bool V4l2MmapDevice::init()
{
    qd() << "init device";

    open("/dev/video0");
    set_format(m_fd);

    //int caps = V4L2_CAP_VIDEO_CAPTURE | V4L2_CAP_STREAMING;

    start();
}

bool V4l2MmapDevice::isReady()
{
    return  ((m_fd != -1)&& (n_buffers != 0));
}

V4l2MmapDevice::~V4l2MmapDevice()
{
    stop();
}


bool V4l2MmapDevice::request_buffer(int fd, int count)
{
    struct v4l2_requestbuffers req = {0};

    req.count   = count;
    req.type    = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory  = V4L2_MEMORY_MMAP;

    if (ioctl(fd, VIDIOC_REQBUFS, &req) == -1)
    {
        qd() << "error VIDIOC_REQBUFS";
        return false;
    }
    return true;
}

bool V4l2MmapDevice::queryBuffers(int fd, int count)
{
    for (int i = 0; i < count; ++i)
    {
        v4l2_buffer buf = {};

        buf.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index  = i;

        int res = ioctl(fd, VIDIOC_QUERYBUF, &buf);

        if (res == -1)
        {
            qd() << "error VIDIOC_QUERYBUF " << i;
            return false;
        }
        auto bufff = mmap (NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, buf.m.offset);

        if (bufff == MAP_FAILED)
        {
            qd() << "error mmap " << i;
        }

        // какая то проверка buf.bytesused могла бы быть

        m_buffers[i].start = bufff;
        m_buffers[i].length = buf.length;
        _bufSize = buf.length;
    }
    return true;
}

bool V4l2MmapDevice::start_streaming(int fd)
{
    unsigned int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    if (ioctl(fd, VIDIOC_STREAMON, &type) == -1)
    {
        qd() << "error VIDIOC_STREAMON";
        return false;
    }
    return true;
}

bool V4l2MmapDevice::queueBuffers(int fd, int count)
{
    for (int i = 0; i < count; ++i)
    {
        v4l2_buffer buf = {};

        buf.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index  = i;

        if (ioctl(fd, VIDIOC_QBUF, &buf) == -1)
        {
            qd() << "error VIDIOC_QBUF " << i;
            return false;
        }
    }
    return true;
}

int V4l2MmapDevice::set_format(int fd)
{
    qd() << "set format...";

    v4l2_format format = {};
    format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    format.fmt.pix.width = 640;
    format.fmt.pix.height = 480;
    format.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
    format.fmt.pix.field = V4L2_FIELD_NONE;

    int res = ioctl(fd, VIDIOC_S_FMT, &format);
    if(res == -1)
    {
        qd() << " error could not set format";
        return false;
    }

    qd() << "set format OK";
    return true;
}

bool V4l2MmapDevice::start() 
{
    qd() << "start device " << _deviceName;

    if (!request_buffer(m_fd, V4L2MMAP_NBBUFFER))
        return false;

    if (!queryBuffers(m_fd, V4L2MMAP_NBBUFFER))
        return false;

    if (!start_streaming(m_fd))
        return false;

    if (!queueBuffers(m_fd, V4L2MMAP_NBBUFFER))
        return false;


    qd() << "start device OK";
    return true;
}

bool V4l2MmapDevice::unmapAndDeleteBuffers()
{
    qd() << "unmap and delete buffers";

    bool result = true;
    for(uint32_t i=0; i< m_buffers.size(); i++)
    {
        if (munmap(m_buffers[i].start, m_buffers[i].length) == -1)
        {
            qd() << "error munmap";
            result = false;
        }
    }

    m_buffers.clear();

    qd() <<  "Mmap buffers deleted";
    return result;
}

bool V4l2MmapDevice::freeBuffers()
{
    qd() << "free buffers";

    n_buffers = 0;

    v4l2_requestbuffers req = {};

    req.count    = 0;
    req.type     = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory   = V4L2_MEMORY_MMAP;

    if (ioctl(m_fd, VIDIOC_REQBUFS, &req) == -1)
    {
        qd() << "error VIDIOC_REQBUFS";
        return false;
    }

    return true;
}

bool V4l2MmapDevice::stop() 
{
    qd() << "stop device " << _deviceName;

    streamOff(m_fd);
    unmapAndDeleteBuffers();
    freeBuffers();

    return true;
}

bool V4l2MmapDevice::open(QString deviceName)
{
    qd() << "open device " << deviceName;

    _deviceName = deviceName;

    m_fd = ::open(deviceName.toStdString().c_str(),  O_RDWR);

    if (m_fd < 0)
    {
        qd() <<  "error cannot open device:" << deviceName << " " << strerror(errno);
        return false;
    }

    qd() << "open device OK";

    return true;
}

void V4l2MmapDevice::close()
{
    if (m_fd != -1)
        ::close(m_fd);

    m_fd = -1;
}

bool V4l2MmapDevice::isReadable(int timeoutMs) const
{
    if (m_fd == -1)
    {
        qd() << "error fd = -1";
    }
    // Use poll to wait for video capture events
    pollfd poll_fds[1];
    poll_fds[0].fd = m_fd;
    poll_fds[0].events = POLLIN;
    const int ret = poll(poll_fds, 1, timeoutMs); // wait up to 1 second for events ( put -1 if it should wait forever )
    return ret == 1;
}

size_t V4l2MmapDevice::bufSize() const
{
     return _bufSize;
}

bool V4l2MmapDevice::streamOff(int fd)
{
    qd() << "stream off";

    v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    if (ioctl(fd, VIDIOC_STREAMOFF, &type) == -1)
    {
        qd() << "error VIDIOC_STREAMOFF";
        return false;
    }

    return true;
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

size_t V4l2MmapDevice::readInternal(char* buffer, size_t bufferSize)
{
    size_t size = 0;
    if (n_buffers > 0)
    {
        v4l2_buffer buf = {};
        buf.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;

        if (-1 == ioctl(m_fd, VIDIOC_DQBUF, &buf))
        {
            perror("VIDIOC_DQBUF");
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

            if (-1 == ioctl(m_fd, VIDIOC_QBUF, &buf))
            {
                perror("VIDIOC_QBUF");
                size = -1;
            }
        }
    }
    return size;
}



