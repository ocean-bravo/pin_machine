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
#include <QMenu>

#include <QtGlobal>

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
    //qd() << "rect " << scRect;
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
    const QString sceneMode = db().value("scene_mode").toString();

    //qd() << "scene mode " << sceneMode;

    if (event->button() == Qt::LeftButton && sceneMode == "select")
    {
        QCursor c = cursor(); c.setShape(Qt::CrossCursor); setCursor(c);

        _origin = event->pos();
        _rb.reset(new QRubberBand(QRubberBand::Rectangle, this));
        _rb->setGeometry(QRect(_origin, QSize()));
        _rb->show();

        auto isItemSelectable = [this](QMouseEvent* event)
        {
            for (QGraphicsItem* item : items(event->pos()))
            {
                if (item->flags() & QGraphicsItem::ItemIsSelectable)
                    return true;
            }
            return false;
        };

        if (isItemSelectable(event))
            QGraphicsView::mousePressEvent(event);

        return;
    }

    if (event->button() == Qt::LeftButton && sceneMode == "manual_path")
    {
        QCursor c = cursor(); c.setShape(Qt::CrossCursor); setCursor(c);
        auto isItemSelectable = [this](QMouseEvent* event)
        {
            for (QGraphicsItem* item : items(event->pos()))
            {
                if (item->flags() & QGraphicsItem::ItemIsSelectable)
                    return true;
            }
            return false;
        };

        if (isItemSelectable(event))
            QGraphicsView::mousePressEvent(event);

        return;
    }

    if (event->button() == Qt::LeftButton)
    {
        _dragMode = true;

        _lastMouseEvent = QMouseEvent(QEvent::MouseMove, event->localPos(), event->windowPos(), event->screenPos(),
                                      event->button(), event->buttons(), event->modifiers());
        setCursor(Qt::ClosedHandCursor);
        event->accept();
        return;
    }
}


void GraphicsView::mouseReleaseEvent(QMouseEvent* event)
{
    QCursor c = cursor(); c.setShape(Qt::ArrowCursor); setCursor(c);

    if (_rb)
    {
        QRect rubberBandRect = QRect(_origin, event->pos()).normalized();
        _rb.reset();

        // Если квадрат 1*1, то мышь не двигали, а просто кликнули.
        // Чтобы получался deselect по клику на блобе. Если событие дойдет до блоба, он поменяет выделение.
        if (rubberBandRect.width() == 1 && rubberBandRect.height() == 1)
        {
            QGraphicsView::mouseReleaseEvent(event);
            return;
        }

        QPainterPath selectionArea;
        selectionArea.addPolygon(mapToScene(rubberBandRect));
        selectionArea.closeSubpath();
        scene()->setSelectionArea(selectionArea, Qt::AddToSelection, Qt::IntersectsItemShape, viewportTransform());

        event->accept();
        return;
    }

    if (_dragMode)
    {
        _dragMode = false;
        event->accept();
        return;
    }
}

void GraphicsView::mouseMoveEvent(QMouseEvent* event)
{
    const QPoint pos = event->pos();

    QJsonObject jo;
    jo.insert("label_number", 0);
    jo.insert("text", QString("%1 %2").arg(toReal3(mapToScene(pos).x())).arg(toReal3(mapToScene(pos).y())));

    db().insert("message", jo);

    if (_dragMode)
    {
        QScrollBar *hBar = horizontalScrollBar();
        QScrollBar *vBar = verticalScrollBar();
        QPoint delta = event->pos() - _lastMouseEvent.pos();
        hBar->setValue(hBar->value() + (isRightToLeft() ? delta.x() : -delta.x()));
        vBar->setValue(vBar->value() - delta.y());
        _lastMouseEvent = QMouseEvent(QEvent::MouseMove, event->localPos(), event->windowPos(), event->screenPos(),
                                      event->button(), event->buttons(), event->modifiers());
        return;
    }

    const QString sceneMode = db().value("scene_mode").toString();

    if (_rb)
    {
        QRect rubberBandRect = QRect(_origin, event->pos()).normalized();
        _rb->setGeometry(rubberBandRect);

        scene()->setSelectionArea(_selArea, Qt::AddToSelection, Qt::IntersectsItemShape, viewportTransform());
        event->accept();
        return;
    }

    QGraphicsView::mouseMoveEvent(event);
}


void GraphicsView::mouseDoubleClickEvent(QMouseEvent *event)
{
    QAbstractScrollArea::mouseReleaseEvent(event);
}

void GraphicsView::contextMenuEvent(QContextMenuEvent* event)
{
    QGraphicsView::contextMenuEvent(event);

    if (event->isAccepted())
        return;

    if (db().value("scene_mode").toString() == "drag")
    {
        QMenu menu;

        menu.addAction(tr("Scan here"), this, [this, event]()
        {
            emit scanPosition(mapToScene(event->pos()));
        });

        const bool blobsHighlightState = db().value("blobs_highlight").toBool();
        QAction* toggleHighlight = menu.addAction(tr("Toggle blobs highlight"), this, [this, blobsHighlightState]()
        {
            db().insert("blobs_highlight", !blobsHighlightState);
        });

        toggleHighlight->setCheckable(true);
        toggleHighlight->setChecked(blobsHighlightState);

        menu.addAction(tr("Add blob here"), this, [this, event]()
        {
            emit addBlob(mapToScene(event->pos()));
        });

        menu.addAction(tr("Calculate path from starting point"), this, [this, event]()
        {
            emit calcPath(db().value("punchpath_start_point").toPointF());
        });

        menu.addAction(tr("Calculate path from here"), this, [this, event]()
        {
            emit calcPath(mapToScene(event->pos()));
        });

        menu.exec(event->globalPos());
    }

    if (db().value("scene_mode").toString() == "manual_path")
    {
        QMenu menu;

        menu.addAction(tr("Reset"), this, [this, event]()
        {
            db().insert("punchpath_manual_reset", true);
        });

        menu.exec(event->globalPos());
    }
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

void GraphicsView::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_S)
    {
        db().insert("scene_mode", QString("select"));
    }

    if (event->key() == Qt::Key_M)
    {
        db().insert("scene_mode", QString("manual_path"));
    }
}

void GraphicsView::keyReleaseEvent(QKeyEvent* /*event*/)
{
    db().insert("scene_mode", QString("drag"));
}

void GraphicsView::focusOutEvent(QFocusEvent* event)
{
    if (event->reason() == Qt::PopupFocusReason)
        return;


    db().insert("scene_mode", QString("drag"));
}
