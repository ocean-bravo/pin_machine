// libv4l2
#include <linux/videodev2.h>
#include <sys/poll.h>

// project
//#include "logger.h"
#include "V4l2Capture.h"
#include "V4l2MmapDevice.h"
#include "V4l2ReadWriteDevice.h"


// -----------------------------------------
//    create video capture interface
// -----------------------------------------
V4l2Capture* V4l2Capture::create(const V4L2DeviceParameters & param)
{
    V4l2Capture* videoCapture = nullptr;
    V4l2Device* videoDevice = nullptr;
	int caps = V4L2_CAP_VIDEO_CAPTURE;
	switch (param.m_iotype)
	{
		case IOTYPE_MMAP: 
			videoDevice = new V4l2MmapDevice(param, V4L2_BUF_TYPE_VIDEO_CAPTURE); 
			caps |= V4L2_CAP_STREAMING;
		break;
		case IOTYPE_READWRITE:
			videoDevice = new V4l2ReadWriteDevice(param, V4L2_BUF_TYPE_VIDEO_CAPTURE); 
			caps |= V4L2_CAP_READWRITE;
		break;
	}
	
	if (videoDevice &&  !videoDevice->init(caps))
	{
		delete videoDevice;
        videoDevice = nullptr;
	}
	
	if (videoDevice)
	{
		videoCapture = new V4l2Capture(videoDevice);
	}	
	return videoCapture;
}

V4l2Capture::V4l2Capture(V4l2Device* device) : V4l2Access(device)
{
}

V4l2Capture::~V4l2Capture() 
{
}

// -----------------------------------------
//    check readability
// -----------------------------------------
bool V4l2Capture::isReadable(int timeoutMs) const
{
	int fd = m_device->getFd();
    // Use poll to wait for video capture events
    pollfd poll_fds[1];
    poll_fds[0].fd = fd;
    poll_fds[0].events = POLLIN;
    const int ret = poll(poll_fds, 1, timeoutMs); // wait up to 1 second for events ( put -1 if it should wait forever )
    return ret == 1;
}

// -----------------------------------------
//    read from V4l2Device
// -----------------------------------------
size_t V4l2Capture::read(char* buffer, size_t bufferSize)
{
	return m_device->readInternal(buffer, bufferSize);
}
