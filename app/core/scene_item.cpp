#include "scene_item.h"

#include <QPainter>

#include "data_bus.h"

#include "common.h"

#include "board_quick_item.h"
#include "image_quick_item.h"
#include "quick_item_triangle.h"

#include <QTimer>
#include <QRandomGenerator>

namespace {

double myrand()
{
    return QRandomGenerator::global()->generateDouble();
}

}

SceneItem::SceneItem(QQuickItem* parent)
    : QQuickItem(parent)
{
    //setAcceptHoverEvents(true);
    //setAcceptedMouseButtons(Qt::AllButtons);
    m_prevPoint = QPoint(-1, -1);

    addBoard();

    setFlag(QQuickItem::ItemHasContents);

    QTimer::singleShot(100, this, [this] () { addBoard(); }) ;
    QTimer::singleShot(300, this, [this] () { addBoard(); }) ;
    QTimer::singleShot(600, this, [this] () { addBoard(); }) ;

    setWidth(200);
    setHeight(200);


}

QImage SceneItem::image() const
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

void SceneItem::addBoard()
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

void SceneItem::addTriangle()
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

QQuickItem* SceneItem::root() const
{
    return _root;
}

void SceneItem::setRoot(QQuickItem* root)
{
    _root = root;
}

void SceneItem::deleteBoards()
{
    every<BoardQuickItem>(childItems(), [](BoardQuickItem* board) { delete board; });
}

void SceneItem::wheelEvent(QWheelEvent *event)
{
    qd() << "wheel event";

    const QPoint angleDelta = event->angleDelta();

    if (angleDelta.x() == 0 && angleDelta.y() != 0) // Вертикальный скролл
    {
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

void SceneItem::setImage(const QImage & img)
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

void SceneItem::mousePressEvent(QMouseEvent *event)
{
    m_prevPoint = event->pos();
}

void SceneItem::mouseMoveEvent(QMouseEvent *event)
{
    auto curPos =event->pos();
    auto offsetPos = curPos - m_prevPoint;
    auto tm = QTransform()
            .translate(offsetPos.x(), offsetPos.y());
    m_prevPoint = curPos;
    m_transform *= tm;
    update();
}

void SceneItem::mouseReleaseEvent(QMouseEvent */*event*/)
{

}

void SceneItem::hoverMoveEvent(QHoverEvent *event)
{
    //QQuickWindow::hoverMoveEvent(event);
}
