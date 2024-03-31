#include "qml_graphics_view.h"
#include "graphics_view.h"

#include "utils.h"


#include <QTimer>
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

    //qd() << "qml constructor";
    //setRenderTarget(QQuickPaintedItem::FramebufferObject);

    QTimer* timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [this]()
    {
        //qd() << "timer render";
        auto scRect = scene()->itemsBoundingRect().toRect();
        const double max = std::max(scRect.width(), scRect.height());
        const int margin = max * 0.05;

        auto re = _view->viewport()->rect();
        //qd() << "viewport rect" << re;


        //_pixmap = QPixmap(scRect.adjusted(-margin,-margin, margin, margin).size());
        // if (!_render)
        // {
        //     _okToRead= false;
            _pixmap = QPixmap(re.size());
            QPainter painter(&_pixmap);
            _view->render(&painter);//, boundingRect(), scRect.adjusted(-margin,-margin, margin, margin), Qt::KeepAspectRatio);
            //_okToRead = true;
       // }
        update();

         // qd () << "width : " <<  _view->viewport()->width();
         // qd () << "height : " << _view->viewport()->height();


    });

    timer->start(50);

    connect(this, &QmlGraphicsView::widthChanged,  this, [this]() { _view->viewport()->setFixedWidth(width());   });
    connect(this, &QmlGraphicsView::heightChanged, this, [this]() { _view->viewport()->setFixedHeight(height()); });
    //_view->setAttribute(Qt::WA_DontShowOnScreen);
    _view->setVisible(true);

}

QmlGraphicsView::~QmlGraphicsView()
{
}

// void QmlGraphicsView::update(const QRect &rect)
// {


//     QQuickPaintedItem::update(rect);
// }


void QmlGraphicsView::paint(QPainter* painter)
{
    //qd() << "qml render";


    //_view->render(painter);

    //scene()->render(painter);
       //view_->render(painter, boundingRect(), view_->viewport()->rect(), Qt::KeepAspectRatio);

    // _render = true;
    // if (_okToRead)
    // {

        painter->drawPixmap(0,0, _pixmap);
    // }

    // _render = false;


   // QPixmap picture(boundingRect().size().toSize());
    //QCPPainter qcpPainter(&picture);

    // _vi->toPainter(&qcpPainter);
    // setRenderTarget(QQuickPaintedItem::FramebufferObject);

    // painter->drawPixmap(QPoint(), picture);
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

void QmlGraphicsView::mousePressEvent(QMouseEvent* e)
{
    qd() << "mouse press event " << e->pos();
    QMouseEvent event(e->type(), e->localPos(),
                      e->windowPos(), e->screenPos(),
                      e->button(), e->buttons(), e->modifiers(), e->source());
    QCoreApplication::sendEvent(_view.data(), &event);
    //routeMouseEvents(event);
}

void QmlGraphicsView::mouseReleaseEvent(QMouseEvent* event)
{
    QCoreApplication::sendEvent(_view.data(), event);

    //routeMouseEvents(event);
}

void QmlGraphicsView::mouseMoveEvent(QMouseEvent* event)
{
    QCoreApplication::sendEvent(_view.data(), event);

    //routeMouseEvents(event);
}

void QmlGraphicsView::mouseDoubleClickEvent(QMouseEvent* event)
{
    QCoreApplication::sendEvent(_view.data(), event);
    //routeMouseEvents(event);
}

void QmlGraphicsView::wheelEvent(QWheelEvent* event)
{
    qd() << "wheel event";
    //routeWheelEvents(event);
    QCoreApplication::sendEvent(_view.data(), event);
}

void QmlGraphicsView::timerEvent(QTimerEvent* /*event*/)
{
    //    _plot->graph(0)->addData(t, U);
    //    _plot->replot();
}

void QmlGraphicsView::contextMenuEvent(QContextMenuEvent *event)
{
    QCoreApplication::sendEvent(_view.data(), event);
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

    QCoreApplication::postEvent(_view.data(), event);
}
