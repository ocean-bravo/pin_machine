#include "data_bus.h"
#include "settings.h"

#include <QImage>

DataBus::DataBus(QObject *parent)
    : QQmlPropertyMap(this, parent)
{
    insert("pixel_size_800", settings().value("pixel_size_800").toDouble());
    insert("pixel_size_1280", settings().value("pixel_size_1280").toDouble());
    insert("pixel_size_2592", settings().value("pixel_size_2592").toDouble());

    insert("x_coord", QString("0.000"));
    insert("y_coord", QString("0.000"));

    insert("image_raw", QImage());
    insert("image_blob", QImage());
    insert("image_circle", QImage());
    insert("image_raw_captured", QImage());
    insert("image_small_blob_captured", QImage());
    insert("image_adapt_threshold_1", QImage());
    insert("image_adapt_threshold_2", QImage());
    insert("pixel_size_test", 0);

    insert("next", "");

    // Сколько кадров нужно выкинуть.
    insert("jpg_frames_throw", 15); // 12 вроде достаточно было
    insert("yuv_frames_throw", 1);  // Достаточно 1, чтобы не было смаза. Не всегда...

    connect(this, &DataBus::valueChanged, this, [this](const QString& key, const QVariant&)
    {
        if (key == "resolution_width")
            emit pixelSizeChanged();
    });
}

DataBus::~DataBus()
{

}

double DataBus::pixInMm() const
{
    int width = value("resolution_width").toInt();

    return value(QString("pixel_size_%1").arg(width)).toDouble();
}

void DataBus::setPixInMm(double size)
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

    if (key.contains("image_"))
        emit imageChanged(key);
}

QVariant DataBus::value(const QString& key) const
{
    _lock.lockForRead();
    QVariant var = QQmlPropertyMap::value(key);
    _lock.unlock();
    return var;
}

DataBusRef DataBus::operator [](const QString& key)
{
    return DataBusRef(key);
}

// QVariant DataBus::updateValue(const QString &key, const QVariant &input)
// {
//     qd() << "key " << key  << " value "  << input;

//         emit valueChanged(key, value);
//     return QQmlPropertyMap::updateValue(key, input);
// }
