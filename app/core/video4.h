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

class Video4Private;
class MjpegHelper;


class Video4 : public QObject
{
    Q_OBJECT

public:
    Video4();
    ~Video4();

    Q_INVOKABLE void changeCamera(int device, int width, int height, QString fourcc);
    Q_INVOKABLE void start();
    Q_INVOKABLE void stop();

    Q_INVOKABLE void capture();

    Q_INVOKABLE void captureSmallRegion(double width);

    Q_INVOKABLE QImage smallRegion();

signals:
    void newImage(QImage);
    void captured(QImage);
    void capturedSmallRegion(QImage);

private:
    Video4Private* _impl = nullptr;
    QThread* _thread = nullptr;

    QImage _smallRegion;
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

signals:
    void newImage(QImage);
    void captured(QImage);
    void capturedSmallRegion(QImage);
    void finished();

    //void imageCaptured(QImage); // Private

private:
    void imageDispatch(QImage img);

    QScopedPointer<V4l2MmapDevice> _videoCapture;

    QMutex _mutex;
    QAtomicInteger<bool> _stop = false;

    QAtomicInteger<bool> _capture = false;
    QAtomicInteger<bool> _captureSmallRegion = false;
    QAtomicInteger<int> _framesToThrowOut = 1;

    QString _currentFourcc;
    MjpegHelper* _jpegDecompressor = nullptr;
    double _smallRegionWidth = 0;

    friend class Video4;
};

