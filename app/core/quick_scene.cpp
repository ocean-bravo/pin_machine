#include "quick_scene.h"

#include <QPainter>

#include "data_bus.h"

#include "common.h"

#include "treewalker.h"

#include "board_quick_item.h"
#include "image_quick_item.h"
#include "quick_item_triangle.h"

#include <QTimer>
#include <QRandomGenerator>

//using ElementWalker = TreeWalker<Element *>;

namespace {



double myrand()
{
    return QRandomGenerator::global()->generateDouble();
}

}

QuickScene::QuickScene(QQuickItem* parent)
    : QQuickItem(parent)
{
    setAcceptHoverEvents(true);
    setAcceptedMouseButtons(Qt::AllButtons);
    m_prevPoint = QPoint(-1, -1);

    addBoard();

    setFlag(QQuickItem::ItemHasContents);

    QTimer::singleShot(100, this, [this] () { addBoard(); }) ;
    QTimer::singleShot(300, this, [this] () { addBoard(); }) ;
    QTimer::singleShot(600, this, [this] () { addBoard(); }) ;

    setWidth(200);
    setHeight(200);
}

qreal QuickScene::zoom() const
{
    return m_zoom;
}

void QuickScene::setZoom(qreal zoom)
{
    if (qFuzzyCompare(m_zoom, zoom))
        return;

    const auto delta = zoom / m_zoom;
    zoomByInternal(delta);

    m_zoom = zoom;
    emit zoomChanged(m_zoom);
}

void QuickScene::zoomBy(qreal scale)
{
    setZoom(m_zoom * scale);
}

void QuickScene::zoomByInternal(qreal scale)
{
    QTransform matrix;
    matrix.scale(scale, scale);

    every<QQuickItem>(childItems(), [&](QQuickItem* item)
    {
        item->setPosition(matrix.map(item->position()));
        item->setWidth(item->width() * scale);
        item->setHeight(item->height() * scale);
    });

    // ElementWalker walker(ElementWalker::PreOrderTraversal);

    // walker.walkItems(root, [&](Element *element) -> ElementWalker::VisitResult
    // {
    //     element->setPos(matrix.map(element->pos()));
    //     element->setWidth(element->width() * scale);
    //     element->setHeight(element->height() * scale);
    //     if (auto transition = qobject_cast<Transition *>(element)) {
    //         transition->setShape(matrix.map(transition->shape()));
    //     }
    //     return ElementWalker::RecursiveWalk;
    // });
}

void QuickScene::wheelEvent(QWheelEvent *event)
{
    qd() << "wheel event";

    const QPoint angleDelta = event->angleDelta();

    if (angleDelta.x() == 0 && angleDelta.y() != 0) // Вертикальный скролл
    {

        // zoomByInternal((float(angleDelta.y())/1200));
        // return;


        QPointF pos = event->position();
        m_scale = 1 + (float(angleDelta.y())/1200);

        setScale(scale() + (float(angleDelta.y())/1200) * scale());
        setTransformOriginPoint(pos);

        // auto tm = QTransform()
        //         .translate(pos.x(), pos.y())
        //         .scale(m_scale, m_scale)
        //         .translate(-pos.x(), -pos.y());
        // m_transform *= tm;
        update();
    }
}




QImage QuickScene::image() const
{
    return QImage();
}

// void SceneItem::paint(QPainter *painter)
// {


//     //     // painter->setTransform(m_transform);

//     //     // if (_image.isNull())
//     //     //     return;

//     //     // int w = width();
//     //     // int h = w * _image.height() / _image.width();

//     //     // //painter->drawImage(QRect(-w/2, -h/2, w, h), _image);
//     //     // painter->drawImage(QRect(0, 0, w, h), _image);

//     //     // // Перекрестие по центру картинки
//     //     // painter->setPen(QPen(Qt::white, 0));
//     //     // painter->drawLine(w/2, 0, w/2, h);
//     //     // painter->drawLine(0, h/2, w, h/2);

//     //painter->save();
//     painter->setPen(QPen(Qt::yellow, 5, Qt::SolidLine));
//     painter->drawRect(QRectF(10, 10, 50, 50));
//     //painter->restore();

// }

void QuickScene::addBoard()
{
    qd() << "add board";

    //runOnThreadWait(this, [this]()
    //{
    //every<BoardQuickItem>(childItems(), [](BoardQuickItem* board) { delete board; });

    _board = new BoardQuickItem(this);
    _board->setVisible(true);
    _board->setParentItem(this);
    _board->setEnabled(true);

    _board->setWidth(100);
    _board->setHeight(100);

    _board->setPosition(QPointF(100, 100));


    //addItem(_board);
    // every<CameraViewItem>(QGraphicsScene::items(), [](CameraViewItem* camera) { delete camera; });
    // addItem(new CameraViewItem);
    //});
}

void QuickScene::addTriangle()
{
    auto tri = new QuickItemTriangle(this);
    tri->setVisible(true);
    tri->setParentItem(this);
    tri->setEnabled(true);

    tri->setWidth(100);
    tri->setHeight(100);

    tri->setPosition(QPointF(400*myrand(), 400*myrand()));

    tri->setColor(QColor(myrand()*255, myrand()*255, myrand()*255, 127));
}

QQuickItem* QuickScene::root() const
{
    return _root;
}

void QuickScene::setRoot(QQuickItem* root)
{
    _root = root;
}

void QuickScene::deleteBoards()
{
    every<BoardQuickItem>(childItems(), [](BoardQuickItem* board) { delete board; });
}



void QuickScene::setImage(const QImage & img)
{
    auto image = new ImageQuickItem(this);

    image->setVisible(true);
    image->setParentItem(this);
    image->setEnabled(true);

    image->setWidth(100);
    image->setHeight(100);
    image->setImage(img);

    image->setPosition(QPointF(200, 200));

    update();
}

void QuickScene::mousePressEvent(QMouseEvent *event)
{
    m_prevPoint = event->pos();
}

void QuickScene::mouseMoveEvent(QMouseEvent *event)
{
    qd() << "mouse pos " << event->pos();
    qd() << "mouse global pos " << event->globalPos();
    qd() << "mouse local pos " << event->localPos();
    qd() << "mouse window pos " << event->windowPos();
    qd() << "mouse screen pos " << event->screenPos();
    qd() << "";


    auto curPos =event->pos();
    auto offsetPos = curPos - m_prevPoint;
    auto tm = QTransform()
            .translate(offsetPos.x(), offsetPos.y());
    m_prevPoint = curPos;
    m_transform *= tm;
    update();
}

void QuickScene::mouseReleaseEvent(QMouseEvent */*event*/)
{

}

void QuickScene::hoverMoveEvent(QHoverEvent *event)
{
    //QQuickWindow::hoverMoveEvent(event);
    qd() << "mouse pos " << event->pos();
    qd() << "";
}
