#pragma once

#include <QString>
#include <QObject>
#include <QImage>
#include "openpnp-capture.h"
#include <vector>
#include <tuple>

#include <QMutex>
#include "video4.h"

class Video4Private;
class V4l2MmapDevice;

class Video4 : public QObject
{
    Q_OBJECT

public:
    Video4();
    ~Video4();

    Q_INVOKABLE void reloadDevices();
    Q_INVOKABLE void changeCamera(quint32 deviceId, quint32 formatId);
    //Q_INVOKABLE void update();

signals:
    void newImage(QImage, QString, QByteArray);

private:
    Video4Private* _impl = nullptr;
    QThread* _thread = nullptr;
};

class Video4Private : public QObject
{
    Q_OBJECT

public:
    Video4Private();
    ~Video4Private();



public slots:
    void reloadDevices();
    void init();
    void update();

    void changeCamera(quint32 deviceId, quint32 formatId);

    //std::vector<DeviceInfo> devicesInfo();

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
    void newImage(QImage, QString, QByteArray);
    void newSize(quint32 width, quint32 height);
    void stopped();

private:
    V4l2MmapDevice* _videoCapture = nullptr;

    QAtomicInteger<bool> _running = false;
};

