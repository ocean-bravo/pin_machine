#pragma once

#include <QGraphicsScene>
#include <QScopedPointer>
#include <QQuickPaintedItem>

class GraphicsView;

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

signals:
    void sceneChanged();

protected:
    void routeMouseEvents( QMouseEvent* event );
    void routeWheelEvents( QWheelEvent* event );

    virtual void mousePressEvent( QMouseEvent* event );
    virtual void mouseReleaseEvent( QMouseEvent* event );
    virtual void mouseMoveEvent( QMouseEvent* event );
    virtual void mouseDoubleClickEvent( QMouseEvent* event );
    virtual void wheelEvent( QWheelEvent *event );

    virtual void timerEvent(QTimerEvent *event);

    virtual void contextMenuEvent(QContextMenuEvent* event);

private:
    QScopedPointer<GraphicsView> _view;

private slots:

};
