#pragma once

#include <QThread>
#include <QObject>
#include <QList>
#include <QSerialPortInfo>
#include <QSerialPort>
#include <QTime>
#include <QScopedPointer>

#include "singleton.h"

class QThread;
class SerialPrivate;

class Serial : public QObject , public Singleton<Serial>
{
    Q_OBJECT

public:
    bool isOpen();

public slots:
    void open();
    void close();
    void write(const QByteArray& msg);
    void setBaudRate(const QString& br);
    void setDataBits(const QString& db);
    void setParity(const QString& p);
    void setStopBits(const QString& sb);
    void setFlowControl(const QString& fc);
    void setPortName(const QString& pn);

signals:
    void data(const QString& msg);
    void message(const QString& msg);
    void connected();
    void disconnected();

protected:
    explicit Serial(QObject* parent = nullptr);
    ~Serial();
    SerialPrivate* const _impl;
    QScopedPointer<QThread> _thread;

    friend class Singleton<Serial>;
};


class SerialPrivate : public QObject
{
    Q_OBJECT

public:
    explicit SerialPrivate(QObject* parent = nullptr);
    virtual ~SerialPrivate();

    bool isOpen() const;

public slots:
    void open();
    void close();
    void write(const QByteArray& msg);
    void setBaudRate(const QString& br);
    void setDataBits(const QString& db);
    void setParity(const QString& p);
    void setStopBits(const QString& sb);
    void setFlowControl(const QString& fc);
    void setPortName(const QString& portName);

signals:
    void data(const QString& msg);
    void message(const QString& msg);
    void connected();
    void disconnected();

private:
    void read();

    QSerialPort* _port = nullptr;
    QByteArray _buffer;
};

inline Serial& serial()
{
    return Serial::instance();
}

