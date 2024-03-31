#pragma once

#include <QMainWindow>
#include <QTimer>
#include <QLabel>
#include <QCheckBox>
#include <QSlider>
#include <QMutex>
#include <vector>

#include "video3.h"


namespace Ui {
class MainWindow2;
}

class MainWindow2 : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow2(QWidget *parent = 0);
    ~MainWindow2();

    //void readCameraSettings();

    /** custom logging function to capture all
        messages/errors from the library.

        Note: this function can be called from _any_ thread,
        i.e. protection must be used when this is called from
        a non-GUI thread.
    */
    void logMessage(uint32_t level, const char *message);

public slots:
    void update(QImage img, QString frameInfo);
    void updateLogMessages();
    void changeCamera();

   // void onAutoExposure(bool state);
   // void onAutoWhiteBalance(bool state);
   // void onAutoGain(bool state);
   // void onAutoFocus(bool state);
   // void onExposureSlider(int value);
   // void onWhiteBalanceSlider(int value);
   // void onGainSlider(int value);
   // void onContrastSlider(int value);
   // void onBrightnessSlider(int value);
   // void onSaturationSlider(int value);
   // void onFocusSlider(int value);
   // void onZoomSlider(int value);
   // void onGammaSlider(int value);
   // void onHueSlider(int value);
   // void onBacklightSlider(int value);
   // void onSharpnessSlider(int value);
   // void onColorEnableSlider(int value);

private:
    bool        m_hasBrightness;
    bool        m_hasExposure;
    bool        m_hasGamma;
    bool        m_hasWhiteBalance;
    bool        m_hasGain;
    bool        m_hasContrast;
    bool        m_hasSaturation;
    bool        m_hasFocus;
    bool        m_hasZoom;
    bool        m_hasHue;
    bool        m_hasSharpness;
    bool        m_hasBacklightcomp;
    bool        m_hasColorEnable;

    QTimer*     m_refreshTimer;


    Video3* _video3;
    Ui::MainWindow2 *ui;
};

