#pragma once

#include <QGraphicsScene>
#include <QScopedPointer>
#include <QQuickPaintedItem>
#include <QWidget>

class GraphicsView;
class MyViewPort;

class QmlGraphicsView : public QQuickPaintedItem
{
    Q_OBJECT

    Q_PROPERTY(QGraphicsScene* scene READ scene WRITE setScene NOTIFY sceneChanged)


public:
    QmlGraphicsView(QQuickItem* parent = 0);
    ~QmlGraphicsView();

    void paint(QPainter* painter);

    QGraphicsScene* scene() const;
    void setScene(QGraphicsScene *scene);

    Q_INVOKABLE void fit();

    //void update(const QRect& rect);
signals:
    void sceneChanged();

protected:
    void mousePressEvent( QMouseEvent* event );
    void mouseReleaseEvent( QMouseEvent* event );
    void mouseMoveEvent( QMouseEvent* event );

    void mouseDoubleClickEvent( QMouseEvent* event );
    void wheelEvent( QWheelEvent *event );

    void contextMenuEvent(QContextMenuEvent* event);

    //bool event(QEvent* ev);

    void hoverMoveEvent(QHoverEvent* event);

private:
    QScopedPointer<GraphicsView> _view;
    QPixmap _pixmap;

    // QAtomicInteger<bool> _okToRead = false;
    // QAtomicInteger<bool> _render = false;
    MyViewPort* _viewport = nullptr;

};


class  MyViewPort : public QWidget
{
    Q_OBJECT

    friend class QmlGraphicsView;
public:
    MyViewPort(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
    ~MyViewPort();

        void mouseMoveEvent( QMouseEvent* event );

};
