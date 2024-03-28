#pragma once

#include <QQuickPaintedItem>

class BoardQuickItem : public QQuickPaintedItem
{
public:
    BoardQuickItem(QQuickItem* parent = nullptr);

    void paint(QPainter* painter) override;
};
