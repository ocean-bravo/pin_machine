#pragma once

#include <QImage>
#include <QObject>
#include <QPainter>
#include <QQuickImageProvider>

#include "../utils.h"


class MyImageProvider : private QObject, public QQuickImageProvider
{
    Q_OBJECT

public:
    MyImageProvider();

    void setPixmap(const QPixmap& pixmap)
    {
        _pixmap = pixmap;
    }

    void setImage(const QImage& image, const QString& id)
    {
        _images.insert(id, image);
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


        return _images.value(id);
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
    void imageChanged(QString);


private:
    QPixmap _pixmap;
    QImage _image;

    QMap<QString, QImage> _images;
};
