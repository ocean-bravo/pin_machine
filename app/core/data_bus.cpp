#include "data_bus.h"

void DataBus::insert(const QString &key, const QVariant &value)
{
    QQmlPropertyMap::insert(key, value);
    emit valueChanged(key, value);
}

DataBus::DataBus(QObject *parent)
    : QQmlPropertyMap(this, parent)
{

}


DataBus::~DataBus()
{

}

