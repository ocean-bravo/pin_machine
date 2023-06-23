#pragma once

#include <QString>
#include <QObject>
#include <QImage>
#include "openpnp-capture.h"
#include <vector>
#include <tuple>

struct DeviceInfo
{
    quint32 device;
    quint32 format;
    QString deviceName;
    QString formatName;
};

class Video3Private;

class Video3 : public QObject
{
    Q_OBJECT

public:
    Video3();
    ~Video3();

    Q_INVOKABLE void reloadDevices();
    Q_INVOKABLE void changeCamera(quint32 deviceId, quint32 formatId);
    Q_INVOKABLE void update();

signals:
    void newImage(QImage, QString, QByteArray);

private:


    Video3Private* const _impl;
    QScopedPointer<QThread> _thread;
};

class Video3Private : public QObject
{
    Q_OBJECT

public:
    Video3Private();
    ~Video3Private();



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

private:
    CapContext  _ctx = nullptr;
    int32_t     _streamId = -1;
    CapFormatInfo           _finfo;
    std::vector<uint8_t>    _frameData;

    std::vector<DeviceInfo> _deviceInfo;
};

