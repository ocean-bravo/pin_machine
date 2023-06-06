#include "udp_appender.h"

#include <QUdpSocket>

UdpAppender::UdpAppender(const QHostAddress& host, quint16 port)
    : _host(host)
    , _port(port)
    , _udpSocket(new QUdpSocket)
{
}

void UdpAppender::append(const QString& msg)
{
    _udpSocket->writeDatagram(msg.toUtf8(), _host, _port);
    // TODO: слишком длинные сообщения делить на несколько датаграмм
}
