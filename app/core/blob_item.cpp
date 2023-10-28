#include "blob_item.h"

#include <QPen>
#include <QPainter>

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

//void BlobItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
//{
//    painter->setPen(pen());
//    painter->drawEllipse(QRectF(-2.5, -2.5, 5, 5));
//    QGraphicsEllipseItem::paint(painter, option, widget);
//}

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
    emit pressed();
    QGraphicsItem::mousePressEvent(event);
}

void BlobItem::highlight()
{
    setPen({Qt::red, 2.5, Qt::SolidLine, Qt::RoundCap});
}

void BlobItem::unhighlight()
{
    setPen({Qt::red, 1.5, Qt::SolidLine, Qt::RoundCap});
}
