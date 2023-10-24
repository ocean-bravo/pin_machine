#include "camera_view_item.h"

#include <QPen>
#include <QPainter>

CameraViewItem::CameraViewItem(QGraphicsItem *parent)
    : QGraphicsRectItem(parent)
{
    setPen({Qt::cyan, 1.5, Qt::SolidLine, Qt::RoundCap});
    setZValue(1); // Отметки поверх миниплат
    setAcceptHoverEvents(true);
}

void CameraViewItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    // элемент выглядит единым целым, прозрачности не накладываются
    painter->setCompositionMode(QPainter::CompositionMode_Source);

    painter->setPen(pen());
//    painter->drawLine(QLineF( 0,  2.5,  0,  5));
//    painter->drawLine(QLineF( 0, -2.5,  0, -5));
//    painter->drawLine(QLineF( 2.5,  0,  5,  0));
//    painter->drawLine(QLineF(-2.5,  0, -5,  0));

    QGraphicsRectItem::paint(painter, option, widget);
}

void CameraViewItem::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
    highlight();
    QGraphicsItem::hoverEnterEvent(event);
}

void CameraViewItem::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
    unhighlight();
    QGraphicsItem::hoverLeaveEvent(event);
}

void CameraViewItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    emit pressed();
    QGraphicsItem::mousePressEvent(event);
}

void CameraViewItem::highlight()
{
    setPen({Qt::cyan, 2.5, Qt::SolidLine, Qt::RoundCap});
}

void CameraViewItem::unhighlight()
{
    setPen({Qt::cyan, 1.5, Qt::SolidLine, Qt::RoundCap});
}
