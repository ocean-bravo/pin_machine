#include "blob_item.h"

#include <QPen>
#include <QPainter>

#include <QGraphicsSceneMouseEvent>

#include <QStyleOptionGraphicsItem>

BlobItem::BlobItem(double x, double y, double dia, QGraphicsItem* parent)
    : QGraphicsEllipseItem(parent)
{
    static const QPen redPen(Qt::red, 0, Qt::SolidLine);
    setPen(redPen);
    setRect(-dia/2, -dia/2, dia, dia);
    setPos(x, y);
    setZValue(1); // Отметки поверх платы
    setAcceptHoverEvents(true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
}

void BlobItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    // Передаю дальше отрисовку невыделенного состояния.
    QStyleOptionGraphicsItem savedOption = *option;
    savedOption.state &= ~QStyle::State_Selected; // сбрасываю состояние выделения


    // Сам отрисую как надо выделенное состояние.
    const bool selected = option->state & QStyle::State_Selected;
//    if (selected)
//    {
//        painter->save();
//        //painter->setBrush(QBrush(Qt::blue));
//        painter->setPen(QPen(Qt::blue, 0));
//        painter->drawPath(shape());
//        painter->restore();
//    }

//    QPen pen = painter->pen();
//    selected ? pen.setColor(Qt::blue) : pen.setColor(Qt::red);
//    painter->setPen(pen);

    QPen p = pen();
    p.setColor(Qt::blue);
    setPen(p);



    QGraphicsEllipseItem::paint(painter, &savedOption, widget);

//    QPen pen = painter->pen();
//    isSelected() ? pen.setColor(Qt::blue) : pen.setColor(Qt::red);
//    painter->setPen(pen);



//    painter->drawEllipse(QRectF(-2.5, -2.5, 5, 5));
//    QGraphicsEllipseItem::paint(painter, option, widget);





//    // Сам отрисую как надо выделенное состояние.
//    const bool selected = option->state & QStyle::State_Selected;
//    if (selected)
//    {
//        painter->save();
//        painter->setBrush(QBrush(Qt::green));
//        painter->setPen(QPen(Qt::red, 0));
//        painter->drawPath(shape());
//        painter->restore();
//    }
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
