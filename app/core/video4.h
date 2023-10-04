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

    Q_INVOKABLE void reloadDevices();
    Q_INVOKABLE void changeCamera(int device, int width, int height, QString fourcc);
    Q_INVOKABLE void update();
    Q_INVOKABLE void start();
    Q_INVOKABLE void stop();

    Q_INVOKABLE void capture();

    Q_INVOKABLE void captureSmallRegion();

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
    void reloadDevices();
    void changeCamera(int device, int width, int height, QString fourcc);
    void update();
    void start();
    void stop();

    void capture();
    void captureSmallRegion();


//    void onAutoExposure(bool state);
//    void onAutoWhiteBalance(bool state);
//    void onAutoGain(bool state);
//    void onAutoFocus(bool state);


//    void setExposure(int value);
//    void setWhiteBalance(int value);
//    void setGain(int value);
//    void setContrast(int value);
//    void setBrightness(int value);
//    void setSaturation(int value);
//    void onFocusSlider(int value);
//    void onZoomSlider(int value);
//    void onGammaSlider(int value);
//    void onHueSlider(int value);
//    void onBacklightSlider(int value);
//    void onSharpnessSlider(int value);
//    void onColorEnableSlider(int value);

signals:
    void newImage(QImage);
    void captured(QImage);
    void capturedSmallRegion(QImage);
    void stopped();

    void imageCaptured(QImage); // Private

private:
    void imageDispatch(QImage img);

    QScopedPointer<V4l2MmapDevice> _videoCapture;

    QAtomicInteger<bool> _running = false;
    QAtomicInteger<bool> _capture = false;
    QAtomicInteger<bool> _captureSmallRegion = false;
    QAtomicInteger<int> _framesToThrowOut = 1;

    QString _currentFourcc;
    MjpegHelper* _jpegDecompressor = nullptr;
};

