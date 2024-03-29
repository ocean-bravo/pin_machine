#pragma once

#include <QImage>
#include <QQuickItem>

class SceneItem : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QImage image READ image WRITE setImage)

    Q_PROPERTY(QQuickItem* root READ root WRITE setRoot)


public:
    explicit SceneItem(QQuickItem *parent = nullptr);
    QImage image() const;
    /*Q_INVOKABLE */void setImage(const QImage& image);

    //void paint(QPainter *painter) override;

    void addBoard();

    QQuickItem* root() const;
    void setRoot(QQuickItem*);


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

    QQuickItem* _board;
    QQuickItem* _root = nullptr;
};
