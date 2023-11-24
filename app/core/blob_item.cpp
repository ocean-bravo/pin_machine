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
    //setFlag(QGraphicsItem::ItemIsMovable, true);
}

void BlobItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    // Передаю дальше отрисовку невыделенного состояния.
    QStyleOptionGraphicsItem savedOption = *option;
    savedOption.state &= ~QStyle::State_Selected; // сбрасываю состояние выделения

    QPen p = pen();
    p.setColor(isFiducial() ? Qt::magenta : Qt::red);
    setPen(p);

    if (isFiducial() || isRealFiducial())
    {
        // элемент выглядит единым целым, прозрачности не накладываются
        painter->setPen(p);
        painter->setCompositionMode(QPainter::CompositionMode_Source);

        double rad = rect().width()/2;

        painter->drawLine(QLineF( 0,  rad,  0,  2*rad));
        painter->drawLine(QLineF( 0, -rad,  0, -2*rad));
        painter->drawLine(QLineF( rad,  0,  2*rad,  0));
        painter->drawLine(QLineF(-rad,  0, -2*rad,  0));
    }

    setBrush(isPunch() ? Qt::blue : QBrush());

    QGraphicsEllipseItem::paint(painter, &savedOption, widget);
}

//QVariant BlobItem::itemChange(GraphicsItemChange change, const QVariant &value)
//{
//    if (change == ItemSelectedHasChanged && scene())
//    {
//        const bool selected = value.toBool();
//        repaintLater();
//    }
//    return QGraphicsItem::itemChange(change, value);
//}

QRectF BlobItem::boundingRect() const
{
    QRectF boundRect = QGraphicsEllipseItem::boundingRect();

    // Увеличиваю bounding rect, всегда, не только когда элемент fiducial, чтобы не оставались артефакты
    // после снятия fiducial свойства
    double rad = rect().width()/2;
    boundRect.adjust(-rad, -rad, rad, rad);

    return boundRect;
}

// Область, чтобы попадать мышью, больше ширины линии, которой нарисован элемент
// TODO: Внутри блоба пустота получается. Т.е. shape пустой внутри. Из за этого могут быть
// проблемы с определением пересечений блобов, если один идеально поместитсся внутри другогого, не задевая контуры.
// Проверить эту тему.
QPainterPath BlobItem::shape() const
{
    QPainterPath path;

    path.addEllipse(rect());

    double rad = rect().width()/2;

    path.moveTo(0, rad);  path.lineTo(0, 2*rad);
    path.moveTo(0, -rad); path.lineTo(0, -2*rad);
    path.moveTo(rad, 0);  path.lineTo(2*rad, 0);
    path.moveTo(-rad, 0); path.lineTo(-2*rad, 0);

    QPainterPathStroker stroker;
    stroker.setWidth(0.3);  // ширина области занимаемая линией
    stroker.setCapStyle(Qt::RoundCap);
    return stroker.createStroke(path);
}

bool BlobItem::isFiducial() const
{
    return data(0).toBool();
}

void BlobItem::setFiducial(bool state)
{
    setData(0, state);
    repaintLater();
}

bool BlobItem::isPunch() const
{
    return isSelected();
}

void BlobItem::setPunch(bool state)
{
    setSelected(state);
    repaintLater();
}

bool BlobItem::isRealFiducial() const
{
    return data(1).toBool();
}

void BlobItem::setRealFiducial(bool state)
{
    setData(1, state);
    state ? setRotation(45) : setRotation(0);
    repaintLater();
}

void BlobItem::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
    {
        QJsonObject jo;
        jo.insert("label_number", 1);
        jo.insert("text", QString("pos: %1 %2").arg(toReal3(scenePos().x())).arg(toReal3(scenePos().y())));
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
    QMenu menu;

    QString menuText = isFiducial() ? tr("Reset fiducial") : tr("Set fiducial");

    menu.addAction(menuText, this, [this]()
    {
        setFiducial(!isFiducial());
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

void BlobItem::repaintLater()
{
    // Надо принудительно перерисовать блоб. Решил вызвать обновление через очередь. Просто так.
    runOnThread(this, [this](){ update(); });
}
