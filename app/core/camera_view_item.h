#pragma once

#include <QGraphicsRectItem>

class CameraViewItem : public QGraphicsRectItem
{
public:
    CameraViewItem(QGraphicsItem* parent = nullptr);

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0) override;
};
