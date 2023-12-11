#pragma once

#include <QImage>
#include <QQuickPaintedItem>

class ImageItem : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(QImage image READ image WRITE setImage)

public:
    explicit ImageItem(QQuickItem *parent = nullptr);
    QImage image() const;
    /*Q_INVOKABLE */void setImage(const QImage& image);

    void paint(QPainter *painter) override;

protected:
    void wheelEvent(QWheelEvent* event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void hoverMoveEvent(QHoverEvent *event);
    void mousePressEvent(QMouseEvent *event);

private:
    QImage _image;
    QPoint m_transOrigin;
    QTransform m_transform;
    QPoint m_prevPoint;
    qreal m_scale = 1.0;
};
