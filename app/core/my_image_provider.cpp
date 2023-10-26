#include "my_image_provider.h"

#include <QtGlobal>

MyImageProvider::MyImageProvider()
    : QObject()
    , QQuickImageProvider(QQuickImageProvider::Image)
{
}

void MyImageProvider::clearCaptured()
{
    _lock.lockForWrite();

    QStringList keysToRemove;
    for (const QString& k : qAsConst(_images).keys())
    {
        if (k.contains("captured_"))
            keysToRemove.append(k);
    }

    for (const QString& k : qAsConst(keysToRemove))
    {
        _images.remove(k);
    }

    _lock.unlock();
}

void MyImageProvider::setPixmap(const QPixmap &pixmap)
{
    _pixmap = pixmap;
}

void MyImageProvider::setImage(const QImage &image, const QString &id)
{
    //qd() << "set image " << id;
    _lock.lockForWrite();
    _images.insert(id, image);
    _lock.unlock();

    emit imageChanged(id);
}

QImage MyImageProvider::requestImage(const QString &id, QSize */*size*/, const QSize &/*requestedSize*/)
{
    //qd() << " request image " << id <<  size << requestedSize;


    //            int width = 100;
    //            int height = 50;

    //            if (size)
    //                *size = QSize(width, height);

    //            QPixmap pixmap(requestedSize.width() > 0 ? requestedSize.width() : width,
    //                           requestedSize.height() > 0 ? requestedSize.height() : height);
    //            pixmap.fill(QColor(id).rgba());
    // write the color name
    //            QPainter painter(&_pixmap);
    //            QFont f = painter.font();
    //            f.setPixelSize(20);
    //            painter.setFont(f);
    //            painter.setPen(Qt::black);
    //            if (requestedSize.isValid())
    //                painter.scale(requestedSize.width() / width, requestedSize.height() / height);
    //            painter.drawText(QRectF(0, 0, width, height), Qt::AlignCenter, id);


    //qd() << "request image " << id;
    _lock.lockForRead();
    QImage img = _images.value(id);
    _lock.unlock();

    return img;
}

QPixmap MyImageProvider::requestPixmap(const QString &id, QSize *size, const QSize &requestedSize)
{

    qd() << " request pixmap " << id <<  size << requestedSize;
    int width = 100;
    int height = 50;

    if (size)
        *size = QSize(width, height);
    QPixmap pixmap(requestedSize.width() > 0 ? requestedSize.width() : width,
                   requestedSize.height() > 0 ? requestedSize.height() : height);
    pixmap.fill(QColor(id).rgba());

    //write the color name
    QPainter painter(&_pixmap);
    QFont f = painter.font();
    f.setPixelSize(20);
    painter.setFont(f);
    painter.setPen(Qt::black);
    if (requestedSize.isValid())
        painter.scale(requestedSize.width() / width, requestedSize.height() / height);
    painter.drawText(QRectF(0, 0, width, height), Qt::AlignCenter, id);

    qd() << " request pixmap " << id <<  size << requestedSize;

    return _pixmap;
}
