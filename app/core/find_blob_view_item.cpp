#include "find_blob_view_item.h"

#include <QPen>
#include <QPainter>

FindBlobViewItem::FindBlobViewItem(QGraphicsItem *parent)
    : QGraphicsRectItem(parent)
{
    setPen({Qt::red, 1, Qt::SolidLine, Qt::FlatCap});
    //setZValue(1000);
}

// void FindBlobViewItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
// {
//     // элемент выглядит единым целым, прозрачности не накладываются
//     painter->setCompositionMode(QPainter::CompositionMode_Source);

//     //painter->setPen(pen());
// //    painter->drawLine(QLineF(0,  -rect().width()/2,  0,  rect().width()));
// //    painter->drawLine(QLineF(0,  -rect().width()/2,  0,  rect().width()));


//     //painter->drawLine(QLineF(-rect().height()/2,  0, rect().height(),  0));
//     int w = rect().width();
//     int h = rect().height();
//     painter->setPen(QPen(Qt::white, 0));
//     painter->drawLine(-w/2, 0, w/2, 0);
//     painter->drawLine(0, -h/2, 0, h/2);

//     QGraphicsRectItem::paint(painter, option, widget);
// }
