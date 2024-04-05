#pragma once

#include <QThread>
#include <QObject>
#include <QSerialPort>
#include <QAtomicInteger>
#include <QScopedPointer>

#include "singleton.h"

class SerialPrivate;

class Serial : public QObject , public Singleton<Serial>
{
    Q_OBJECT
    Q_PROPERTY(bool isOpen READ isOpen NOTIFY isOpenChanged)

public:
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
    void setPortName(const QString& pn);

signals:
    void data(const QString& msg);
    void message(const QString& msg);
    void connected();
    void disconnected();
    void isOpenChanged();

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
    void isOpenChanged();

private:
    void read();

    QAtomicInteger<bool> _isOpen = false;
    QSerialPort* _port = nullptr;
    QByteArray _buffer;

    friend class Serial;
};

inline Serial& serial()
{
    return Serial::instance();
}
