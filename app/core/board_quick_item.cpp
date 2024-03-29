#include "board_quick_item.h"

#include <QPen>
#include <QPainter>

#include "utils.h"

BoardQuickItem::BoardQuickItem(QQuickItem* parent)
    : QQuickPaintedItem(parent)
{
    setFlag(ItemHasContents);
}

void BoardQuickItem::paint(QPainter* painter)
{
    qd() << "board paint";
    //painter->save();
    painter->setPen(QPen(Qt::red, 5, Qt::SolidLine));
    painter->drawRect(QRectF(0, 0, 40, 40));
    //painter->restore();
}
