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

    if (isFiducial())
        setBrush(Qt::magenta);
    else if (isSelected())
        setBrush(Qt::blue);
    else
        setBrush(QBrush());

    if (isWork())
        setBrush(Qt::yellow);

    QGraphicsEllipseItem::paint(painter, &savedOption, widget);
}

QVariant BlobItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == ItemSelectedHasChanged && scene())
    {
        const bool selected = value.toBool();

        if (!selected)
            setFiducal(false);

        // Надо принудительно перерисовать блоб. Решил вызвать обновление через очередь. Просто так.
        runOnThread(this, [this](){ update(); });
    }
    return QGraphicsItem::itemChange(change, value);
}

bool BlobItem::isFiducial() const
{
    return data(0).toBool();
}

void BlobItem::setFiducal(bool state)
{
    setData(0, state);
}

bool BlobItem::isWork() const
{
    return data(1).toBool();
}

void BlobItem::setWork(bool state)
{
    setData(1, state);
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
        setSelected(!isSelected());
    QGraphicsEllipseItem::mousePressEvent(event);
}

void BlobItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    Q_UNUSED(event)
    // В базовом классе какое-то действие, которое снимает выделение
    // Не прокидываю дальше событие
    return;
}

void BlobItem::contextMenuEvent(QGraphicsSceneContextMenuEvent* event)
{
    if (!isSelected())
        return;

    QMenu menu;

    QString menuText = isFiducial() ? tr("Reset fiducial") : tr("Set fiducial");

    menu.addAction(menuText, this, [this]()
    {
        setFiducal(!isFiducial());

        // Надо принудительно перерисовать блоб. Решил вызвать обновление через очередь. Просто так.
        runOnThread(this, [this](){ update(); });
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
