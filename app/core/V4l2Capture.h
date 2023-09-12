#pragma once

#include <iostream>

class V4l2MmapDevice;
struct V4L2DeviceParameters;

class V4l2Capture
{		
public:
    V4l2Capture(const V4L2DeviceParameters& param);
    ~V4l2Capture();

    size_t read(char* buffer, size_t bufferSize);
    bool   isReadable(int timeoutMs = 1000) const;

private:
    V4l2MmapDevice* m_device = nullptr;
};
