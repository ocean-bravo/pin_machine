#pragma once

#include <QQuickPaintedItem>

class GraphicsView;

class PlaceholderQuickItem : public QQuickPaintedItem
{
    Q_OBJECT

    Q_PROPERTY(qreal width READ width WRITE setWidth NOTIFY widthChanged FINAL)
    Q_PROPERTY(qreal height READ height WRITE setHeight NOTIFY heightChanged FINAL)

public:
    PlaceholderQuickItem(QQuickItem* parent = nullptr);

    //void paint(QPainter* painter) override;
    //QSGNode* updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *);

    void paint(QPainter* painter) override;

protected:

    void hoverEnterEvent(QHoverEvent *event) override;
     void hoverLeaveEvent(QHoverEvent *event) override;
    // void hoverMoveEvent(QHoverEvent *event) override;

     void geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry) override;

private:
    GraphicsView* _mw;
    QQuickWindow* _win = nullptr;
};
