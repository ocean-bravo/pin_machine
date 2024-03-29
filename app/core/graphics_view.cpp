#include "graphics_view.h"
//#include "core/utils.h"
//#include "core/common.h"
//#include "core/message.h"

#include "utils.h"
#include "data_bus.h"
#include <QTimer>
#include <QJsonObject>
#include <QWheelEvent>
#include <QGraphicsTextItem>
#include <QScrollBar>

GraphicsView::GraphicsView(QWidget* parent)
    : QGraphicsView(parent)
{
    setRenderHint(QPainter::Antialiasing);
    setBackgroundBrush(QBrush(Qt::black));
    scale(1.0, -1.0); // Отражение сцены по вертикали, для привычного направления оси Y вверх

    // Нужно для зума в позицию мыши
    setTransformationAnchor(QGraphicsView::NoAnchor);
    setResizeAnchor(QGraphicsView::NoAnchor);

    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // Оставил на память. Так разрешаются события hover
    //setAttribute(Qt::WA_Hover);

    setMouseTracking(true); // Для отображения координат курсора мыши
}

void GraphicsView::fit()
{
    const QRectF scRect = scene()->itemsBoundingRect();
    //const QRectF scRect(QPointF(0,0), _sizeToFit);
    const double max = std::max(scRect.width(), scRect.height());
    const int margin = max * 0.05;

    fitInView(scRect.adjusted(-margin,-margin, margin, margin), Qt::KeepAspectRatio); // Это подгоняет размер платы

    // Не дает подвинуть сцену за пределы максиплаты
    //setSceneRect(scRect.adjusted(-margin,-margin, margin, margin)); // Это центрует миниплату
}

void GraphicsView::setSizeToFit(QSizeF size)
{
    _sizeToFit = size;
}

//void GraphicsView::zoom(double factor)
//{
//    //    scale(factor, factor);
//    //    centerOn(target_scene_pos);
//    //    QPointF delta_viewport_pos = target_viewport_pos - QPointF(viewport()->width() / 2.0, viewport()->height() / 2.0);
//    //    QPointF viewport_center = mapFromScene(target_scene_pos) - delta_viewport_pos;
//    //    centerOn(mapToScene(viewport_center.toPoint()));
//}

////https://stackoverflow.com/questions/19113532/qgraphicsview-zooming-in-and-out-under-mouse-position-using-mouse-wheel
void GraphicsView::wheelEvent(QWheelEvent* event)
{
    QPointF pos = event->position();

    int numDegrees = event->angleDelta().y() / 8;
    int numSteps = numDegrees / 15; // see QWheelEvent documentation

    QPointF oldPos = mapToScene(pos.toPoint());

    qreal factor = 1.0 + qreal(numSteps) / 10.0;
    scale(factor, factor);

    QPointF newPos = mapToScene(pos.toPoint());
    QPointF delta = newPos - oldPos;

    translate(delta.x(), delta.y());

    emit sizeChanged(QSize()); // Для маркера осей
}

void GraphicsView::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::MiddleButton)
    {
        fit();
        event->accept();
        return;
    }

    if (event->button() == Qt::LeftButton && event->modifiers() & Qt::CTRL)
    {
        //qd() << "rubber band";

        _origin = event->pos();
        //qd() << "rubber band pos " << _origin;
        _rb.reset(new QRubberBand(QRubberBand::Rectangle, this));
        _rb->setGeometry(QRect(_origin, QSize()));
        _rb->show();

        QCursor c = cursor(); c.setShape(Qt::CrossCursor); setCursor(c);
        event->accept();

        auto isItemSelectable = [this](QMouseEvent* event)
        {
            for (QGraphicsItem* item : items(event->pos()))
            {
                if (item->flags() & QGraphicsItem::ItemIsSelectable)
                    return true;
            }
            return false;
        };

        if (!isItemSelectable(event))
        {
            QGraphicsView::mousePressEvent(event);
            return;
        }

        // Если передать сигнал дальше, т.е. без return, то миниплата выделится по клику, она сама это сделает.
        // Но если в месте клика будет элемент, который можно двигать, например fake pin, надо чтобы он не двигался.
        // Для этого, событие mouseMove дальше не прокидывается, когда есть резиновый квадрат.
        // Как альтернативный вариант, выделять миниплату прямо здесь - создать функцию с вызовом setSelectionArea
        // размером с 1 пиксель.
        //QAbstractScrollArea::mousePressEvent(event);

    }

    if (event->button() == Qt::LeftButton && event->modifiers() == Qt::NoModifier)
    {
        _dragMode = true;
        _lastMouseEvent = QMouseEvent(QEvent::MouseMove, event->localPos(), event->windowPos(), event->screenPos(),
                                                           event->button(), event->buttons(), event->modifiers());
        setCursor(Qt::ClosedHandCursor);
        event->accept();
        return;
    }

    if ((event->button() == Qt::LeftButton || event->button() == Qt::RightButton) && (event->modifiers() == Qt::NoModifier))
    {
        // Кликнули мимо всех элементов
        return;
    }

    QGraphicsView::mousePressEvent(event);
}

void GraphicsView::mouseDoubleClickEvent(QMouseEvent *event)
{
    QAbstractScrollArea::mouseReleaseEvent(event);
}

void GraphicsView::mouseReleaseEvent(QMouseEvent* event)
{
    _rb.reset();

    QCursor c = cursor(); c.setShape(Qt::ArrowCursor); setCursor(c);

    if (_dragMode)
    {
        _dragMode = false;
        //setCursor(Qt::ArrowCursor);
        event->accept();
        return;
    }

    QGraphicsView::mouseReleaseEvent(event);
}

void GraphicsView::mouseMoveEvent(QMouseEvent* event)
{
    const QPoint pos = event->pos();

    QJsonObject jo;
    jo.insert("label_number", 0);
    jo.insert("text", QString("%1 %2").arg(toReal3(mapToScene(pos).x())).arg(toReal3(mapToScene(pos).y())));

    db().insert("message", jo);


//    if (_dragMode)
//    {

//        event->ignore();

//    }

    if (_dragMode)
    {
//        horizontalScrollBar()->setValue(horizontalScrollBar()->value() - (event->x() - _panStartX));
//        verticalScrollBar()->setValue(verticalScrollBar()->value() - (event->y() - _panStartY));
////        _panStartX = event->x();
////        _panStartY = event->y();
//        event->accept();


//        QPointF delta = _lastScreenPos - event->screenPos();
//        int newX = horizontalScrollBar()->value() + delta.x();
//        int newY = verticalScrollBar()->value() + delta.y();
//        horizontalScrollBar()->setValue(newX);
//        verticalScrollBar()->setValue(newY);


                    QScrollBar *hBar = horizontalScrollBar();
                    QScrollBar *vBar = verticalScrollBar();
                    QPoint delta = event->pos() - _lastMouseEvent.pos();
                    hBar->setValue(hBar->value() + (isRightToLeft() ? delta.x() : -delta.x()));
                    vBar->setValue(vBar->value() - delta.y());
                    _lastMouseEvent = QMouseEvent(QEvent::MouseMove, event->localPos(), event->windowPos(), event->screenPos(),
                                                                   event->button(), event->buttons(), event->modifiers());
        return;
    }



    if (_rb)
    {
        QRect rubberBandRect = QRect(_origin, event->pos()).normalized();
        _rb->setGeometry(rubberBandRect);

        QPainterPath selectionArea;
        selectionArea.addPolygon(mapToScene(rubberBandRect));
        selectionArea.closeSubpath();
        scene()->setSelectionArea(selectionArea, Qt::AddToSelection, Qt::IntersectsItemShape, viewportTransform());
        event->accept();
        return;
    }

    QGraphicsView::mouseMoveEvent(event);
}

void GraphicsView::resizeEvent(QResizeEvent* event)
{
    _dragMode = false;
    emit sizeChanged(event->size());
    QGraphicsView::resizeEvent(event);
}

bool GraphicsView::event(QEvent* event)
{
    //qd() << "view event type " << event->type();
    return QGraphicsView::event(event);
}
