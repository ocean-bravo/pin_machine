#include "data_bus.h"

DataBus::DataBus(QObject *parent)
    : QQmlPropertyMap(this, parent)
{
    insert("pixel_size_800", 0.017);
    insert("pixel_size_1280", 0.0107);
    insert("pixel_size_2592", 0.00524);

    connect(this, &DataBus::valueChanged, this, [this](const QString& key, const QVariant&)
    {
        if (key == "resolution_width")
            emit pixelSizeChanged();
    });
}

DataBus::~DataBus()
{

}

double DataBus::pixelSize() const
{
    int width = value("resolution_width").toInt();

    return value(QString("pixel_size_%1").arg(width)).toDouble();
}

void DataBus::setPixelSize(double size)
{
    int width = value("resolution_width").toInt();

    insert(QString("pixel_size_%1").arg(width), size);
    emit pixelSizeChanged();
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


