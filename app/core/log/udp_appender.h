#pragma once

#include "appender.h"

#include <QHostAddress>
#include <QScopedPointer>

class QUdpSocket;

class UdpAppender : public Appender
{
public:
    UdpAppender(const QHostAddress& host, quint16 port);
    void append(const QString& msg);

private:
    QHostAddress _host;
    quint16 _port;
    QScopedPointer<QUdpSocket> _udpSocket;
};
