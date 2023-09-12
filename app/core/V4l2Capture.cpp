// libv4l2
#include <linux/videodev2.h>
#include <sys/poll.h>

// project
//#include "logger.h"
#include "utils.h"
#include "V4l2Device.h"
#include "V4l2Capture.h"
#include "V4l2MmapDevice.h"
#include "V4l2ReadWriteDevice.h"


V4l2Capture::V4l2Capture(const V4L2DeviceParameters &param)
{
//    int caps = V4L2_CAP_VIDEO_CAPTURE | V4L2_CAP_STREAMING;
//    m_device = new V4l2MmapDevice(param, V4L2_BUF_TYPE_VIDEO_CAPTURE);

//    if (!m_device->init(caps))
//    {
//        delete m_device;
//        m_device = nullptr;
//    }
}

V4l2Capture::~V4l2Capture()
{

    delete m_device;
}

//bool V4l2Capture::isReadable(int timeoutMs) const
//{
//    int  = m_device->m_fd;

//    if (fd == -1)
//    {
//        qd() << "error fd = -1";
//    }
//    // Use poll to wait for video capture events
//    pollfd poll_fds[1];
//    poll_fds[0].fd = fd;
//    poll_fds[0].events = POLLIN;
//    const int ret = poll(poll_fds, 1, timeoutMs); // wait up to 1 second for events ( put -1 if it should wait forever )
//    return ret == 1;
//}

//V4l2Device *V4l2Capture::device()
//{
//    return m_device;
//}

size_t V4l2Capture::read(char* buffer, size_t bufferSize)
{
    //return m_device->readInternal(buffer, bufferSize);
}
