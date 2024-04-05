#include "serial.h"
#include "utils.h"

#include <QSerialPortInfo>
#include <QSerialPort>


SerialPrivate::SerialPrivate(QObject* parent)
    : QObject(parent)
    , _port(new QSerialPort(this))
{
    connect(_port, &QSerialPort::readyRead, this, &SerialPrivate::read, Qt::QueuedConnection);
    connect(_port, &QSerialPort::errorOccurred, this, [this](QSerialPort::SerialPortError error)
    {
        qd() << "serial port error: " << error;
        const bool wasOpen = _isOpen;
        _isOpen = _port->isOpen();

        if (wasOpen == _isOpen)
            return;

        emit isOpenChanged();

        _isOpen ? emit connected() : emit disconnected();

    });
}

SerialPrivate::~SerialPrivate()
{}

void SerialPrivate::read()
{
    const QByteArray a = _port->readAll();
    //qd() << a;

    if (!a.isEmpty())
    {
        emit data(a);
    }
}

void SerialPrivate::setPortName(const QString& pn)
{
    _port->setPortName(pn);
    //    const QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();

    //    qd() << "set port Name:  " << portName;

    //    for (const QSerialPortInfo& item : ports)
    //    {
    //        qd() << item.portName();
    //    }

    //    for (const QSerialPortInfo& item : ports)
    //    {
    //        if (portName.contains(item.portName()))
    //        {
    //            _port->setPortName(item.portName());
    //            break;
    //        }
    //    }
}

void SerialPrivate::setBaudRate(const QString& br)
{
    _port->setBaudRate(br.toInt());
}

void SerialPrivate::setDataBits(const QString& db)
{
    _port->setDataBits(QSerialPort::DataBits(db.toInt()));
}

void SerialPrivate::setParity(const QString& p)
{
    QSerialPort::Parity parity = QSerialPort::NoParity;
    if (p == "Even")
    {
        parity = QSerialPort::EvenParity;
    }
    else if (p == "Odd")
    {
        parity = QSerialPort::OddParity;
    }
    else if (p == "Space")
    {
        parity = QSerialPort::SpaceParity;
    }
    else if (p == "Mark")
    {
        parity = QSerialPort::MarkParity;
    }
    _port->setParity(parity);
}

void SerialPrivate::setStopBits(const QString& sb)
{
    QSerialPort::StopBits stopBits = QSerialPort::OneStop;
    if (sb == "1.5")
    {
        stopBits = QSerialPort::OneAndHalfStop;
    }
    else if (sb == "2")
    {
        stopBits = QSerialPort::TwoStop;
    }
    _port->setStopBits(stopBits);
}

void SerialPrivate::setFlowControl(const QString& fc)
{
    QSerialPort::FlowControl flowControl = QSerialPort::NoFlowControl;
    if (fc == "Hardware")
    {
        flowControl = QSerialPort::HardwareControl;
    }
    else if (fc == "Software")
    {
        flowControl = QSerialPort::SoftwareControl;
    }
    _port->setFlowControl(flowControl);
}

void SerialPrivate::open()
{
    if (_port->open(QSerialPort::ReadWrite))
    {
        _isOpen = true;
        emit isOpenChanged();
        emit connected();
        emit message(_port->portName() + " port is opened");
    }
    else
    {
        emit message(_port->portName() + " port is not opened");
    }
}

void SerialPrivate::close()
{
    if (!_port->isOpen())
        return;

    _port->close();
    _isOpen = false;
    emit isOpenChanged();
    emit disconnected();
    emit message(_port->portName() + " port is closed");
}

void SerialPrivate::write(const QByteArray& msg)
{
    //qd() << "write " << msg;
    _port->write(msg);
}

Serial::Serial(QObject* parent)
    : QObject(parent)
    , _impl(new SerialPrivate)
    , _thread(new QThread)
{
    connect(_impl, &SerialPrivate::data,          this, &Serial::data, Qt::QueuedConnection);
    connect(_impl, &SerialPrivate::message,       this, &Serial::message, Qt::QueuedConnection);
    connect(_impl, &SerialPrivate::connected,     this, &Serial::connected, Qt::QueuedConnection);
    connect(_impl, &SerialPrivate::disconnected,  this, &Serial::disconnected, Qt::QueuedConnection);
    connect(_impl, &SerialPrivate::isOpenChanged, this, &Serial::isOpenChanged, Qt::QueuedConnection);

    connect(_thread.data(), &QThread::finished, _impl, &QObject::deleteLater);

    _impl->moveToThread(_thread.data());
    _thread->start();
}

Serial::~Serial()
{
    close();
    _thread->quit();
    _thread->wait(1000);
}

bool Serial::isOpen() const
{
    return _impl->_isOpen;
}

void Serial::setBaudRate(const QString& br)
{
    QMetaObject::invokeMethod(_impl, "setBaudRate", Qt::QueuedConnection, Q_ARG(QString, br));
}

void Serial::setDataBits(const QString& db)
{
    QMetaObject::invokeMethod(_impl, "setDataBits", Qt::QueuedConnection, Q_ARG(QString, db));
}

void Serial::setParity(const QString& p)
{
    QMetaObject::invokeMethod(_impl, "setParity", Qt::QueuedConnection, Q_ARG(QString, p));
}

void Serial::setStopBits(const QString& sb)
{
    QMetaObject::invokeMethod(_impl, "setStopBits", Qt::QueuedConnection, Q_ARG(QString, sb));
}

void Serial::setFlowControl(const QString& fc)
{
    QMetaObject::invokeMethod(_impl, "setFlowControl", Qt::QueuedConnection, Q_ARG(QString, fc));
}

void Serial::setPortName(const QString& pn)
{
    QMetaObject::invokeMethod(_impl, "setPortName", Qt::QueuedConnection,  Q_ARG(QString, pn));
}

void Serial::write(const QByteArray& msg)
{
    QMetaObject::invokeMethod(_impl, "write", Qt::QueuedConnection, Q_ARG(QByteArray, msg));
}

void Serial::open()
{
    QMetaObject::invokeMethod(_impl, "open", Qt::QueuedConnection);
}

void Serial::close()
{
    QMetaObject::invokeMethod(_impl, "close", Qt::QueuedConnection);
}
