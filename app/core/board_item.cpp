#include "board_item.h"

#include <QPen>

BoardItem::BoardItem(QGraphicsItem *parent)
    : QGraphicsRectItem(parent)
{
    setPen({Qt::green, 1, Qt::SolidLine});
    setRect(0, 0, 300, 300);
}
