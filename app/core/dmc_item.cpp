#include "dmc_item.h"

#include <QPen>
#include <QPainter>

DmcItem::DmcItem(QPointF pos, QGraphicsItem *parent)
    : QGraphicsEllipseItem(parent)
{
    setPen({Qt::cyan, 1.5, Qt::SolidLine, Qt::RoundCap});
    setRect(-5,-5, 10, 10);
    setPos(pos);
    setZValue(1); // Отметки поверх миниплат
    setAcceptHoverEvents(true);
}

void DmcItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    // элемент выглядит единым целым, прозрачности не накладываются
    painter->setCompositionMode(QPainter::CompositionMode_Source);

    painter->setPen(pen());
    painter->drawLine(QLineF( 0,  2.5,  0,  5));
    painter->drawLine(QLineF( 0, -2.5,  0, -5));
    painter->drawLine(QLineF( 2.5,  0,  5,  0));
    painter->drawLine(QLineF(-2.5,  0, -5,  0));
    QGraphicsEllipseItem::paint(painter, option, widget);
}

void DmcItem::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
    highlight();
    QGraphicsItem::hoverEnterEvent(event);
}

void DmcItem::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
    unhighlight();
    QGraphicsItem::hoverLeaveEvent(event);
}

void DmcItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    emit pressed();
    QGraphicsItem::mousePressEvent(event);
}

void DmcItem::highlight()
{
    setPen({Qt::cyan, 2.5, Qt::SolidLine, Qt::RoundCap});
}

void DmcItem::unhighlight()
{
    setPen({Qt::cyan, 1.5, Qt::SolidLine, Qt::RoundCap});
}
