#include "blob_item.h"

#include "utils.h"
#include "utils2.h"
#include "data_bus.h"

#include <QPen>
#include <QPainter>
#include <QJsonObject>


#include <QMenu>
#include <QGraphicsSceneMouseEvent>

#include <QStyleOptionGraphicsItem>

BlobItem::BlobItem(double x, double y, double dia, QGraphicsItem* parent)
    : QGraphicsEllipseItem(parent)
{
    static QPen redPen(Qt::red, _nonhighlightedThickness, Qt::SolidLine);
    redPen.setCosmetic(true);
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
//    const bool selected = option->state & QStyle::State_Selected;
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

//    QPen p = pen();
//    p.setColor(selected ? Qt::blue : Qt::red);
//    setPen(p);

    QGraphicsEllipseItem::paint(painter, &savedOption, widget);

//    QPen pen = painter->pen();
//    isSelected() ? pen.setColor(Qt::blue) : pen.setColor(Qt::red);
//    painter->setPen(pen);



//    painter->drawEllipse(QRectF(-2.5, -2.5, 5, 5));
//    QGraphicsEllipseItem::paint(painter, option, widget);


    if (_fiducial)
        setBrush(Qt::magenta);
    else if (isSelected())
        setBrush(Qt::blue);
    else
        setBrush(QBrush());


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

QVariant BlobItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == ItemSelectedHasChanged && scene())
    {
        const bool selected = value.toBool();

        if (!selected)
            _fiducial = false;

        // Надо принудительно перерисовать блоб. Решил вызвать обновление через очередь. Просто так.
        runOnThread(this, [this](){ update(); });

//        if (_fiducial)
//            setBrush(Qt::magenta);
//        else if (isSelected())
//            setBrush(Qt::blue);
//        else
//            setBrush(QBrush());


    }
    return QGraphicsItem::itemChange(change, value);
}


void BlobItem::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
    {
        QJsonObject jo;
        jo.insert("label_number", 1);
        jo.insert("text", QString("pos: %1 %2").arg(toReal3(pos().x())).arg(toReal3(pos().y())));
        db().insert("message", jo);
    }

    {
        QJsonObject jo;
        jo.insert("label_number", 2);
        jo.insert("text", QString("dia: %1").arg(toReal3(rect().width())));
        db().insert("message", jo);
    }

    highlight();
    QGraphicsItem::hoverEnterEvent(event);
}

void BlobItem::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
    {
        QJsonObject jo {{"label_number", 1}};
        db().insert("message", jo);
    }

    {
        QJsonObject jo {{"label_number", 2}};
        db().insert("message", jo);
    }

    unhighlight();
    QGraphicsItem::hoverLeaveEvent(event);
}

void BlobItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    if (event->button() == Qt::LeftButton && event->modifiers() & Qt::CTRL)
    {
        qd() << " is selected " << isSelected();
        setSelected(!isSelected());
    }

    QGraphicsEllipseItem::mousePressEvent(event);
}

void BlobItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    // В базовом классе какое-то действие, которое снимает выделение
    // Не прокидываю дальше событие
    return;

    //QGraphicsEllipseItem::mouseReleaseEvent(event);
}

void BlobItem::contextMenuEvent(QGraphicsSceneContextMenuEvent* event)
{
    if (!isSelected())
        return;

    QMenu menu;

    QString menuText = _fiducial ? tr("Reset fiducial") : tr("Set fiducial");

    menu.addAction(menuText, this, [this]()
    {
        _fiducial = !_fiducial;

        // Надо принудительно перерисовать блоб. Решил вызвать обновление через очередь. Просто так.
        runOnThread(this, [this](){ update(); });

//        if (_fiducial)
//            setBrush(Qt::magenta);
//        else if (isSelected())
//            setBrush(Qt::blue);
//        else
//            setBrush(QBrush());
    });

    menu.exec(event->screenPos());
}

void BlobItem::highlight()
{
    QPen p = pen();
    p.setWidthF(_highlightedThickness);
    setPen(p);
}

void BlobItem::unhighlight()
{
    QPen p = pen();
    p.setWidthF(_nonhighlightedThickness);
    setPen(p);
}
