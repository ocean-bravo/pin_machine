#pragma once

#include <QString>
#include <QObject>
#include <QScopedPointer>
#include <QImage>
#include "openpnp-capture.h"
#include <vector>
#include <tuple>

#include <QMutex>
#include "V4l2MmapDevice.h"

#include "singleton.h"

class Video4Private;
class MjpegHelper;


class Video4 : public QObject, public Singleton<Video4>
{
    Q_OBJECT

public:
    Q_INVOKABLE void changeCamera(int device, int width, int height, QString fourcc);
    Q_INVOKABLE void start();
    Q_INVOKABLE void stop();

    Q_INVOKABLE void capture();

    Q_INVOKABLE void captureSmallRegion(double width);

    QImage smallRegion();


    Q_INVOKABLE void reloadDevices();

signals:
    void newImage(QImage);
    void captured(QImage);
    void capturedSmallRegion(QImage);

private:
    Video4();
    ~Video4();

    Video4Private* _impl = nullptr;
    QThread* _thread = nullptr;

    QImage _smallRegion;

    friend class Singleton<Video4>;
};

class Video4Private : public QObject
{
    Q_OBJECT

public:

public slots:
    void init();
    void changeCamera(int device, int width, int height, QString fourcc);
    void update();

    void capture();
    void captureSmallRegion(double width);

    void reloadDevices();

signals:
    void newImage(QImage);
    void captured(QImage);
    void capturedSmallRegion(QImage);
    void finished();

    //void imageCaptured(QImage); // Private

private:
    void imageDispatch(QImage img);

    QScopedPointer<MyDriver> _videoCapture;

    QMutex _mutex;
    QAtomicInteger<bool> _stop = false;

    QAtomicInteger<bool> _capture = false;
    QAtomicInteger<bool> _captureSmallRegion = false;
    QAtomicInteger<int> _framesToThrowOut = 1;

    QString _currentFourcc;
    MjpegHelper* _jpegDecompressor = nullptr;
    double _smallRegionWidthMm = 0;

    friend class Video4;
};

inline Video4& video()
{
    return Video4::instance();
}
