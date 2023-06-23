#pragma once

#include <QImage>
#include <QObject>
#include <QPainter>
#include <QMutex>
#include <QQuickImageProvider>
#include <QReadWriteLock>

#include "utils.h"

class MyImageProvider : public QObject, public QQuickImageProvider
{
    Q_OBJECT

public:
    MyImageProvider();

    Q_INVOKABLE void clearCaptured();

    void setPixmap(const QPixmap& pixmap);
    void setImage(const QImage& image, const QString& id);
    QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize) override;
    QPixmap requestPixmap(const QString &id, QSize *size, const QSize &requestedSize) override;

signals:
    void imageChanged(QString id);

private:
    QPixmap _pixmap;
    QImage _image;
    QReadWriteLock _lock;
    QMap<QString, QImage> _images;
};
