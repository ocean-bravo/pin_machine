#pragma once

#include <turbojpeg.h>
#include <stdint.h>
#include <stdlib.h> // size_t

class MjpegHelper
{
public:
    MjpegHelper();

    virtual ~MjpegHelper();

    /* Decompress a JPEG contained in the buffer.
        The width and height of the output buffer are for
        sanity checking only. If the JPEG does not match
        the buffer size, the function will return false.
    */
    bool decompressFrame(const uint8_t *inBuffer, size_t inBytes, uint8_t *outBuffer, uint32_t outBufWidth, uint32_t outButHeight);

protected:
    tjhandle _decompressHandle;
};
