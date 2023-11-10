#include "data_bus.h"

double DataBus::pixelSize() const
{
    return value("pixel_size").toDouble();
}

void DataBus::insert(const QString& key, const QVariant& value)
{
    _lock.lockForWrite();
    QQmlPropertyMap::insert(key, value);
    _lock.unlock();

    emit valueChanged(key, value);
}

QVariant DataBus::value(const QString& key) const
{
    _lock.lockForRead();
    QVariant var = QQmlPropertyMap::value(key);
    _lock.unlock();
    return var;
}

DataBus::DataBus(QObject *parent)
    : QQmlPropertyMap(this, parent)
{

}


DataBus::~DataBus()
{

}

