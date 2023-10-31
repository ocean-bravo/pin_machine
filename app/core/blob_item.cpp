#include "blob_item.h"

#include <QPen>
#include <QPainter>

#include <QGraphicsSceneMouseEvent>

BlobItem::BlobItem(double x, double y, double dia, QGraphicsItem* parent)
    : QGraphicsEllipseItem(parent)
{
    static const QPen redPen(Qt::red, 0, Qt::SolidLine);
    setPen(redPen);
    setRect(-dia/2, -dia/2, dia, dia);
    setPos(x, y);
    setZValue(1); // Отметки поверх платы
    setAcceptHoverEvents(true);
}

void BlobItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    QPen pen = painter->pen();
    isSelected() ? pen.setColor(Qt::blue) : pen.setColor(Qt::red);
    painter->setPen(pen);



//    painter->drawEllipse(QRectF(-2.5, -2.5, 5, 5));
    QGraphicsEllipseItem::paint(painter, option, widget);
}

void BlobItem::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
    highlight();
    QGraphicsItem::hoverEnterEvent(event);
}

void BlobItem::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
    unhighlight();
    QGraphicsItem::hoverLeaveEvent(event);
}

void BlobItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    if (event->button() == Qt::LeftButton && event->modifiers() & Qt::CTRL)
    {
        setSelected(!isSelected());
    }

    emit pressed();
    QGraphicsEllipseItem::mousePressEvent(event);
}

void BlobItem::highlight()
{
    static const QPen redPen(Qt::red, 0.1, Qt::SolidLine);
    setPen(redPen);
}

void BlobItem::unhighlight()
{
    static const QPen redPen(Qt::red, 0, Qt::SolidLine);
    setPen(redPen);
}
