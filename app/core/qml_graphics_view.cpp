#include "qml_graphics_view.h"
#include "graphics_view.h"

#include "utils.h"


#include <QTimer>
#include <QDebug>
#include <QWheelEvent>
#include <QGuiApplication>
#include <QApplication>

#include <utility>

QmlGraphicsView::QmlGraphicsView(QQuickItem* parent)
    : QQuickPaintedItem(parent)
    , _view(new GraphicsView)
{
    setFlag(QQuickItem::ItemHasContents, true);
    setFlag(ItemAcceptsInputMethod, true);
    setAcceptHoverEvents(true);
    setAcceptedMouseButtons(Qt::AllButtons);

    _viewport = new MyViewPort;
    _view->setViewport(_viewport);

    //_viewport->setMouseTracking(true);


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
    _view->setVisible(false);

    // QTimer::singleShot(5000, [this]()
    // {
    //     _view->setParent(QApplication::topLevelAt(0,0));
    // });


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

//This is function to override:
void QmlGraphicsView::hoverMoveEvent(QHoverEvent* event)
{
    if (event->pos() == event->oldPos())
        return;

    QMouseEvent* e1 = new QMouseEvent(QEvent::MouseMove, event->pos(),
                                         Qt::NoButton, Qt::NoButton,
                                         Qt::NoModifier);
    QMouseEvent* e2 = new QMouseEvent(QEvent::MouseMove, event->pos(),
                                         Qt::NoButton, Qt::NoButton,
                                         Qt::NoModifier);
    QMouseEvent* e3 = new QMouseEvent(QEvent::MouseMove, event->pos(),
                                         Qt::NoButton, Qt::NoButton,
                                         Qt::NoModifier);
    //e->accept();
    _viewport->mouseMoveEvent(e1);
    _view->mouseMoveEvent(e2);
    QCoreApplication::postEvent(_view.data(), e3);

    ///qd() << "qml view hover event " << event->pos();

    QQuickItem::hoverMoveEvent(event);
}

void QmlGraphicsView::mousePressEvent(QMouseEvent* event)
{
    ///qd() << "mouse press event " << event->pos();
    _view->mousePressEvent(event);
}

void QmlGraphicsView::mouseReleaseEvent(QMouseEvent* event)
{
    _view->mouseReleaseEvent(event);
}

void QmlGraphicsView::mouseMoveEvent(QMouseEvent* event)
{
//    _view->mouseMoveEvent(event);

    ///qd() << "qml view move event " << event;
    _view->mouseMoveEvent(event);

}

void QmlGraphicsView::mouseDoubleClickEvent(QMouseEvent* event)
{
    _view->mouseDoubleClickEvent(event);
}

void QmlGraphicsView::wheelEvent(QWheelEvent* event)
{
    ///qd() << "wheel event";
    _view->wheelEvent(event);
}

void QmlGraphicsView::contextMenuEvent(QContextMenuEvent *event)
{
    _view->contextMenuEvent(event);
}

// bool QmlGraphicsView::event(QEvent* ev)
// {
//     return _view->event(ev);
// }

MyViewPort::MyViewPort(QWidget* parent, Qt::WindowFlags f)
    : QWidget (parent, f)
{
    qd() << "constructed viewport";
    setMouseTracking(true);
}

MyViewPort::~MyViewPort()
{
    qd() << "destructed viewport";
}

void MyViewPort::mouseMoveEvent(QMouseEvent* event)
{
    ///qd() << "viewport event " << event->localPos();

    QWidget::mouseMoveEvent(event);
}
