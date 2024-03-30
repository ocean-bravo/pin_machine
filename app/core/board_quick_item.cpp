#include "board_quick_item.h"

#include <QPen>
#include <QPainter>

#include "utils.h"

#include <QSGSimpleRectNode>

BoardQuickItem::BoardQuickItem(QQuickItem* parent)
    : QQuickItem(parent)
{
    setFlag(ItemHasContents);
    setAcceptHoverEvents(true);
    setWidth(40);
    setHeight(40);
}

// void BoardQuickItem::paint(QPainter* painter)
// {
//     qd() << "board paint";
//     painter->save();
//     painter->setPen(QPen(Qt::red, 5, Qt::SolidLine));
//     painter->drawRect(QRectF(0, 0, width(), height()));
//     painter->restore();
// }

QSGNode* BoardQuickItem::updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *)
{
    auto *node = static_cast<QSGSimpleRectNode *>(oldNode);
    if (!node)
    {
        node = new QSGSimpleRectNode();
    }
    node->setColor(QColor("red"));
    node->setRect(boundingRect());
    //emit updated();
    return node;
}

void BoardQuickItem::hoverEnterEvent(QHoverEvent* event)
{
    qd() << "hover event enter";
}

void BoardQuickItem::hoverLeaveEvent(QHoverEvent* event)
{
    qd() << "hover event leave";
}
