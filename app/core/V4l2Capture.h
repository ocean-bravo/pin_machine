#pragma once

#include "V4l2Access.h"

class V4l2Capture : public V4l2Access
{		
public:
    static V4l2Capture* create(const V4L2DeviceParameters & param);
    virtual ~V4l2Capture();

    size_t read(char* buffer, size_t bufferSize);
    bool   isReadable(int timeoutMs = 1000) const;

protected:
    V4l2Capture(V4l2Device* device);
};
