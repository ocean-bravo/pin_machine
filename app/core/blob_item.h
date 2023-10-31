#pragma once

#include <QGraphicsEllipseItem>

class BlobItem : public QObject, public QGraphicsEllipseItem
{
    Q_OBJECT

public:
    BlobItem(double x, double y, double dia, QGraphicsItem* parent = nullptr);

    //void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0) override;

signals:
    void pressed();

protected:
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;

    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

private:
    void highlight();
    void unhighlight();

    QColor _selectedColor = Qt::blue;
    QColor _nonselectedColor = Qt::red;
};
