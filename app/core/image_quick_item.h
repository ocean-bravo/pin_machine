#pragma once

#include <QQuickItem>
#include <QImage>

class ImageQuickItem : public QQuickItem
{
public:
    ImageQuickItem(QQuickItem* parent = nullptr);

    //void paint(QPainter* painter) override;
    QSGNode* updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *);

    void setImage(QImage image);

protected:

    void hoverEnterEvent(QHoverEvent *event) override;
     void hoverLeaveEvent(QHoverEvent *event) override;
    // void hoverMoveEvent(QHoverEvent *event) override;

private:
     QImage _image;
};
