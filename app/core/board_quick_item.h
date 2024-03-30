#pragma once

#include <QQuickItem>

class BoardQuickItem : public QQuickItem
{
public:
    BoardQuickItem(QQuickItem* parent = nullptr);

    //void paint(QPainter* painter) override;
    QSGNode* updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *);

protected:

    void hoverEnterEvent(QHoverEvent *event) override;
     void hoverLeaveEvent(QHoverEvent *event) override;
    // void hoverMoveEvent(QHoverEvent *event) override;
};
