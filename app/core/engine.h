#pragma once

#include <QObject>
#include <QScopedPointer>

class QQmlApplicationEngine;
class Settings;
class Serial;

class Engine : public QObject
{
    Q_OBJECT

public:
    explicit Engine(QObject* parent = nullptr);
    ~Engine();

signals:
    void readyToCreateQmlEngine();
    void usbPortFound();

private:
    void createQmlEngine();
    void findUsbSerialPort();

    QScopedPointer<QQmlApplicationEngine> _qmlEngine;
    QScopedPointer<Serial> _serial;

    QString _usbSerialPortName;
};
