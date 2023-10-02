#include "mjpeghelper.h"
#include "utils.h"

MjpegHelper::MjpegHelper()
{
    _decompressHandle = tjInitDecompress();
}

MjpegHelper::~MjpegHelper()
{
    tjDestroy(_decompressHandle);
}

bool MjpegHelper::decompressFrame(const uint8_t *inBuffer, size_t inBytes, uint8_t *outBuffer,
    uint32_t outBufWidth, uint32_t outBufHeight)
{
    // note: the jpeg-turbo library apparently uses a non-const
    // buffer pointer to the incoming JPEG data.
    // Hopefully, the lib does not change the JPEG data buffer.
    // so, for now, we'll cast it to a non-const pointer.
    // Yes, that is completely dirty... I'm not happy with it either.

    uint8_t *jpegPtr = const_cast<uint8_t*>(inBuffer);
    int32_t width, height, jpegSubsamp;
    
    tjDecompressHeader2(_decompressHandle, jpegPtr, inBytes, &width, &height, &jpegSubsamp);
    if ((width != outBufWidth) || (height != outBufHeight))
    {
        qd() << "error: tjDecompressHeader2 failed: " << tjGetErrorStr() << "\n";
        return false;
    }
    else
    {
        //qd() << QString("info: MJPG: %1 %2 size %3 bytes\n").arg(width).arg(height).arg(inBytes);
    }

    if (tjDecompress2(_decompressHandle, jpegPtr, inBytes, outBuffer, width, 0/*pitch*/, height, TJPF_RGB, TJFLAG_FASTDCT) != 0)
    {
        // A lot of cameras produce incorrect but decodable JPEG data
        // and produce warnings that fill the console,
        // such as 'extraneous bytes before marker' etc.
        //
        // To avoid cluttering the console, we suppress the warnings
        // and errors completely.. :-/
        //
        // FIXME: the following disabled code only works for
        // very recent libjpeg-turbo libraries that aren't common on systems
        // yet..
        return true;
    }

    return true;
}
