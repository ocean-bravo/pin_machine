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

class Video3 : public QObject
{
    Q_OBJECT

public:
    Video3();
    ~Video3();
    void init();

public slots:
    void update();

    void changeCamera(quint32 device, quint32 format);

    std::vector<DeviceInfo> devicesInfo();

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
    CapContext  _ctx;
    int32_t     _streamId;
    CapFormatInfo           _finfo;
    std::vector<uint8_t>    _frameData;

    std::vector<DeviceInfo> _deviceInfo;
};

