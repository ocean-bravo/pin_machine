#pragma once

#include <QThread>
#include <QObject>
#include <QList>
#include <QSerialPortInfo>
#include <QSerialPort>
#include <QTime>
#include <QScopedPointer>

class QThread;
class SerialPrivate;

class Serial : public QObject
{
    Q_OBJECT

public:
    explicit Serial(QObject* parent = nullptr);
    ~Serial();

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
    SerialPrivate* const _impl;
    QScopedPointer<QThread> _thread;
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



