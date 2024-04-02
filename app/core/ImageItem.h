#pragma once

#include <QImage>
#include <QQuickPaintedItem>

class ImageItem : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(QImage image READ image WRITE setImage)
    Q_PROPERTY(QColor crossColor READ crossColor WRITE setCrossColor)

public:
    explicit ImageItem(QQuickItem *parent = nullptr);
    QImage image() const;
    /*Q_INVOKABLE */void setImage(const QImage& image);

    void paint(QPainter *painter) override;

    void setCrossColor(QColor color);
    QColor crossColor() const;


    Q_INVOKABLE void clear();

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
    QColor _crossColor = Qt::white;
};
