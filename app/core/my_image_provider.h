#pragma once

#include <QImage>
#include <QObject>
#include <QPainter>
#include <QMutex>
#include <QQuickImageProvider>

#include "utils.h"

#include <QReadWriteLock>



class MyImageProvider : public QObject, public QQuickImageProvider
{
    Q_OBJECT

public:
    MyImageProvider();

    void clearCaptured();

    void setPixmap(const QPixmap& pixmap)
    {
        _pixmap = pixmap;
    }

    void setImage(const QImage& image, const QString& id)
    {
        //qd() << "set image " << id;
        _lock.lockForWrite();
        _images.insert(id, image);
        _lock.unlock();

        emit imageChanged(id);
    }

    QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize) override
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

    QPixmap requestPixmap(const QString &id, QSize *size, const QSize &requestedSize) override
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

signals:
    void imageChanged(QString id);


private:
    QPixmap _pixmap;
    QImage _image;

    QReadWriteLock _lock;

    QMap<QString, QImage> _images;
};
