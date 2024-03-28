#include "ImageItem.h"

#include <QPainter>

#include "video4.h"
#include "data_bus.h"

ImageItem::ImageItem(QQuickItem *parent)
    : QQuickPaintedItem(parent)
{
    //setAcceptHoverEvents(true);
    setAcceptedMouseButtons(Qt::AllButtons);
    m_prevPoint = QPoint(-1, -1);
}

QImage ImageItem::image() const
{
    return QImage();
}

void ImageItem::paint(QPainter *painter)
{
    painter->setTransform(m_transform);

    if (_image.isNull())
        return;

    int w = width();
    int h = w * _image.height() / _image.width();

    //painter->drawImage(QRect(-w/2, -h/2, w, h), _image);
    painter->drawImage(QRect(0, 0, w, h), _image);

    // Перекрестие по центру картинки
    painter->setPen(QPen(Qt::white, 0));
    painter->drawLine(w/2, 0, w/2, h);
    painter->drawLine(0, h/2, w, h/2);
}

void ImageItem::wheelEvent(QWheelEvent *event)
{
    qd() << "image item wheel event";
    const QPoint angleDelta = event->angleDelta();

    if (angleDelta.x() == 0 && angleDelta.y() != 0) // Вертикальный скролл
    {
        QPointF pos = event->position();
        m_scale = 1 + (float(angleDelta.y())/1200);
        auto tm = QTransform()
                .translate(pos.x(), pos.y())
                .scale(m_scale, m_scale)
                .translate(-pos.x(), -pos.y());
        m_transform *= tm;
        update();
    }
}

void ImageItem::setImage(const QImage &image)
{
    _image = image;
    update();
}

void ImageItem::mousePressEvent(QMouseEvent *event)
{
    m_prevPoint = event->pos();
}

void ImageItem::mouseMoveEvent(QMouseEvent *event)
{
    auto curPos =event->pos();
    auto offsetPos = curPos - m_prevPoint;
    auto tm = QTransform()
            .translate(offsetPos.x(), offsetPos.y());
    m_prevPoint = curPos;
    m_transform *= tm;
    update();
}

void ImageItem::mouseReleaseEvent(QMouseEvent */*event*/)
{

}

void ImageItem::hoverMoveEvent(QHoverEvent *event)
{
    QQuickPaintedItem::hoverMoveEvent(event);
}
