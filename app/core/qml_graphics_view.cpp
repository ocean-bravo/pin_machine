#include "qml_graphics_view.h"
#include "graphics_view.h"

#include "utils.h"

#include <QDebug>
#include <QWheelEvent>

#include <utility>

QmlGraphicsView::QmlGraphicsView(QQuickItem* parent)
    : QQuickPaintedItem(parent)
    , _view(new GraphicsView)
{
    setFlag(QQuickItem::ItemHasContents, true);
    setAcceptedMouseButtons(Qt::AllButtons);

    // connect(this, &QQuickPaintedItem::widthChanged, this, &QmlGraphicsView::updateCustomPlotSize);
    // connect(this, &QQuickPaintedItem::heightChanged, this, &QmlGraphicsView::updateCustomPlotSize);

    qd() << "qml constructor";
    //setRenderTarget(QQuickPaintedItem::FramebufferObject);
}

QmlGraphicsView::~QmlGraphicsView()
{
}

void QmlGraphicsView::paint(QPainter* painter)
{
    qd() << "qml render";

    _view->render(painter);
}

QGraphicsScene *QmlGraphicsView::scene() const
{
    return _view->scene();
}

void QmlGraphicsView::setScene(QGraphicsScene *scene)
{

    qd() << "qml set scene";
    _view->setScene(scene);
}

void QmlGraphicsView::fit()
{
    qd() << "qml fit";

    _view->fit();
}

void QmlGraphicsView::routeMouseEvents(QMouseEvent* event)
{
    QMouseEvent* newEvent = new QMouseEvent(event->type(), event->localPos(), event->button(), event->buttons(), event->modifiers());
    QCoreApplication::postEvent(_view.data(), newEvent);
}

void QmlGraphicsView::mousePressEvent(QMouseEvent* event)
{
    routeMouseEvents(event);
}

void QmlGraphicsView::mouseReleaseEvent(QMouseEvent* event)
{
    routeMouseEvents(event);
}

void QmlGraphicsView::mouseMoveEvent(QMouseEvent* event)
{
    routeMouseEvents(event);
}

void QmlGraphicsView::mouseDoubleClickEvent(QMouseEvent* event)
{
    routeMouseEvents(event);
}

void QmlGraphicsView::wheelEvent(QWheelEvent* event)
{
    routeWheelEvents(event);
}

void QmlGraphicsView::timerEvent(QTimerEvent* /*event*/)
{
    //    _plot->graph(0)->addData(t, U);
    //    _plot->replot();
}

void QmlGraphicsView::contextMenuEvent(QContextMenuEvent *event)
{
    QCoreApplication::postEvent(_view.data(), event);
}

void QmlGraphicsView::routeWheelEvents(QWheelEvent* event)
{
    // Без понятия зачем копируется событие
#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
    QWheelEvent* newEvent = new QWheelEvent( event->pos(), event->delta(), event->buttons(), event->modifiers(), event->orientation() );
#endif

#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
    QWheelEvent* newEvent = new QWheelEvent(event->position(), event->globalPosition(), event->pixelDelta(), event->angleDelta(), event->buttons(), event->modifiers(), event->phase(), event->inverted(), event->source());
#endif

    QCoreApplication::postEvent(_view.data(), newEvent);
}
