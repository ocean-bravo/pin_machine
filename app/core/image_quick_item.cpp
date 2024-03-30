#include "image_quick_item.h"

#include <QPen>
#include <QPainter>

#include "utils.h"

#include <QSGSimpleRectNode>
#include <QSGImageNode>
#include <QQuickWindow>


ImageQuickItem::ImageQuickItem(QQuickItem* parent)
    : QQuickItem(parent)
{
    setFlag(ItemHasContents);
    setAcceptHoverEvents(true);
    setWidth(40);
    setHeight(40);
}

// void BoardQuickItem::paint(QPainter* painter)
// {
//     qd() << "board paint";
//     painter->save();
//     painter->setPen(QPen(Qt::red, 5, Qt::SolidLine));
//     painter->drawRect(QRectF(0, 0, width(), height()));
//     painter->restore();
// }

QSGNode* ImageQuickItem::updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *)
{
    auto *node = static_cast<QSGImageNode *>(oldNode);
    if (!node)
    {
        node = window()->createImageNode();
        node->setOwnsTexture(true);
        qsgnode_set_description(node, QStringLiteral("windowthumbnail"));
        node->setFiltering(QSGTexture::Linear);
    }

    node->setTexture(window()->createTextureFromImage(_image));

    const QSize size(_image.size().scaled(boundingRect().size().toSize(), Qt::KeepAspectRatio));
    const qreal x = boundingRect().x() + (boundingRect().width() - size.width()) / 2;
    const qreal y = boundingRect().y() + (boundingRect().height() - size.height()) / 2;

    node->setRect(QRectF(QPointF(x, y), size));

    return node;
}

void ImageQuickItem::setImage(QImage image)
{
    _image = image;
}

void ImageQuickItem::hoverEnterEvent(QHoverEvent* event)
{
    qd() << "hover event enter";
}

void ImageQuickItem::hoverLeaveEvent(QHoverEvent* event)
{
    qd() << "hover event leave";
}
