#pragma once

#include <QImage>
#include <QQuickItem>

class QuickScene : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QImage image READ image WRITE setImage)

    Q_PROPERTY(QQuickItem* root READ root WRITE setRoot)

    Q_PROPERTY(QImage image READ image WRITE setImage)

public:
    explicit QuickScene(QQuickItem *parent = nullptr);


    qreal zoom() const;
    void setZoom(qreal zoom);

    void zoomBy(qreal scale);
    void zoomByInternal(qreal scale);


    QImage image() const;
    void setImage(const QImage& image);

    //void paint(QPainter *painter) override;

    Q_INVOKABLE void addBoard();

    Q_INVOKABLE void addTriangle();

    QQuickItem* root() const;
    void setRoot(QQuickItem*);

    Q_INVOKABLE void deleteBoards();

signals:
    void zoomChanged(qreal zoom);


protected:
    void wheelEvent(QWheelEvent* event);
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event);
    void hoverMoveEvent(QHoverEvent *event);
    void mousePressEvent(QMouseEvent *event);

private:

    qreal m_zoom;

    QImage _image;
    QPoint m_transOrigin;
    QTransform m_transform;
    QPoint m_prevPoint;
    qreal m_scale = 1.0;

    QQuickItem* _board;
    QQuickItem* _root = nullptr;
};
