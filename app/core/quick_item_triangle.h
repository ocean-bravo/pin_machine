#pragma once

#include <QQuickItem>

class QuickItemTriangle : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)

public:
    QuickItemTriangle(QQuickItem *parent = Q_NULLPTR);
    QColor color() const;
    void setColor(const QColor &color);

    bool contains(const QPointF &point) const;

signals:
    void colorChanged();

protected:
    QSGNode *updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *updatePaintNodeData);

    void hoverEnterEvent(QHoverEvent *event) override;
    void hoverLeaveEvent(QHoverEvent *event) override;

private:
    QColor _color;
    bool m_needUpdate;


};
