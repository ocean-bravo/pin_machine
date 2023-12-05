#include "ImageItem.h"

#include <QPainter>

#include "video4.h"
#include "data_bus.h"

ImageItem::ImageItem(QQuickItem *parent)
    : QQuickPaintedItem(parent)
{
    connect(&video(), &Video4::newImage, this, [this](QImage img)
    {
        m_image = img;
        update();
    });

    setAcceptHoverEvents(true);
    setAcceptedMouseButtons(Qt::AllButtons);
    m_prevPoint = QPoint(-1, -1);
}

void ImageItem::paint(QPainter *painter)
{
    painter->setTransform(m_transform);

    if (m_image.isNull())
        return;

    int w = width();
    int h = w * m_image.height() / m_image.width();

    painter->drawImage(QRect(-w/2, -h/2, w, h), m_image);

    // Перекрестие по центру картинки
    painter->setPen(QPen(Qt::white, 0));
    painter->drawLine(0, h/2, 0, -h/2);
    painter->drawLine(-w/2, 0, w/2, 0);
}

void ImageItem::wheelEvent(QWheelEvent *event)
{
    if (event->orientation() == Qt::Vertical)
    {
        auto pos = event->pos();
        m_scale = 1 + (float(event->delta())/1200);
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
    m_image = image;
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

void ImageItem::mouseReleaseEvent(QMouseEvent *event)
{

}

void ImageItem::hoverMoveEvent(QHoverEvent *event)
{
    QQuickPaintedItem::hoverMoveEvent(event);
}
