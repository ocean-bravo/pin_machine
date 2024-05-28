#include "data_bus.h"
#include "settings.h"

#include <QImage>

DataBus::DataBus(QObject *parent)
    : QQmlPropertyMap(this, parent)
{
    insert("homing_status", QString("Not ready"));

    QStringList resolutions;
    for (const QString& key : settings().allKeys())
    {
        if (key.contains("pixel_size_"))
            resolutions.append(key);
    }

    for (const QString& res : resolutions)
    {
        insert(res, settings().value(res).toDouble());
    }

    insert("x_coord", QString("????"));
    insert("y_coord", QString("????"));
    insert("z_coord", QString("????"));
    insert("w_coord", QString("????"));

    insert("live_preview_image_raw", QImage());
    insert("live_preview_image_blob", QImage());
    insert("live_preview_image_circle", QImage());
    insert("live_preview_image_raw_captured", QImage());
    insert("live_preview_image_small_blob_captured", QImage());
    insert("live_preview_image_adapt_threshold", QImage());
    insert("live_preview_image_adapt_corr", QImage());

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

QString DataBus::messageBox() const
{
    return value("messagebox").toString();
}

void DataBus::setMessageBox(QString msg)
{
    insert("messagebox", msg);
    emit messageBoxChanged();
}

QString DataBus::messageBoxResult() const
{
    return value("messagebox_result").toString();
}

void DataBus::setMessageBoxResult(QString msg)
{
    insert("messagebox_result", msg);
    emit messageBoxResultChanged();
}

double DataBus::pixInMm() const
{
    int width = value("resolution_width").toInt();

    QVariant val = value(QString("pixel_size_%1").arg(width));

    if (val.isNull())
    {
        qd() << "warning: no pixel size for resolution " << width;
        return 1.0; // Индикатор того, что нет pixInMm для данного разрешения
    }

    return val.toDouble();
}

void DataBus::setPixInMm(double size)
{
    int width = value("resolution_width").toInt();

    insert(QString("pixel_size_%1").arg(width), size);
    emit pixelSizeChanged();
}

double DataBus::widthToPixInMm(int width)
{
    QVariant val = value(QString("pixel_size_%1").arg(width));

    if (val.isNull())
        return 1.0; // Индикатор того, что нет pixInMm для данного разрешения

    return val.toDouble();
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
