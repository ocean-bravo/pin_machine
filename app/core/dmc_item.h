#pragma once

#include <QGraphicsEllipseItem>

class DmcItem : public QObject, public QGraphicsEllipseItem
{
    Q_OBJECT

public:
    DmcItem(QPointF pos, QGraphicsItem* parent = nullptr);

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0) override;

    QString data; // некие данные, которые должны быть сравнены с данными, считанными с платы с этой метки

signals:
    void pressed();

protected:
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;

private:
    void highlight();
    void unhighlight();
};
