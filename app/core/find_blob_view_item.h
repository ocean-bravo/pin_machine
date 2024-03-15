#pragma once

#include <QGraphicsRectItem>

class FindBlobViewItem : public QGraphicsRectItem
{
public:
    FindBlobViewItem(QGraphicsItem* parent = nullptr);

    // /void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0) override;
};
