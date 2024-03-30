#include "quick_item_triangle.h"

#include "utils.h"

#include <QPen>
#include <QSGGeometry>
#include <QSGGeometryNode>
#include <QSGFlatColorMaterial>

using Node = QSGNode;
using Geo = QSGGeometry;
using GeoNode = QSGGeometryNode;
using FlatColorMaterial = QSGFlatColorMaterial;


QuickItemTriangle::QuickItemTriangle(QQuickItem *parent)
    : QQuickItem(parent)
    , _color(Qt::red)
    , _needUpdate(true)
{
    setFlag(QQuickItem::ItemHasContents);
    setAcceptHoverEvents(true);
}



QSGNode* QuickItemTriangle::updatePaintNode(Node *oldNode, QQuickItem::UpdatePaintNodeData *updatePaintNodeData)
{
    Q_UNUSED(updatePaintNodeData)
    GeoNode *root = static_cast<GeoNode *>(oldNode);

    if(!root)
    {
        root = new GeoNode;
        Geo *geometry = new Geo(Geo::defaultAttributes_Point2D(), 3);

        //geometry->setDrawingMode(GL_TRIANGLE_FAN); //сплошной треугольник

        geometry->setDrawingMode(GL_LINE_LOOP);
        geometry->setLineWidth(5);

        geometry->vertexDataAsPoint2D()[0].set(width() / 2, 0);
        geometry->vertexDataAsPoint2D()[1].set(width(), height());
        geometry->vertexDataAsPoint2D()[2].set(0, height());

        root->setGeometry(geometry);
        root->setFlags(Node::OwnsGeometry | Node::OwnsMaterial); // нода теперь владеет этими объектами и при самоуничтожении неплохо было бы уничтожить и эти объекты.
    }

    if(_needUpdate)
    {
        FlatColorMaterial *material = new FlatColorMaterial;
        material->setColor(_color);
        root->setMaterial(material);
        _needUpdate = false;
    }

    return root;
}

QColor QuickItemTriangle::color() const
{
    return _color;
}

void QuickItemTriangle::setColor(const QColor &color)
{
    if(_color != color)
    {
        _color = color;
        _needUpdate = true;
        update();
        emit colorChanged();
    }
}

bool QuickItemTriangle::contains(const QPointF& point) const
{

}

void QuickItemTriangle::hoverEnterEvent(QHoverEvent* event)
{
    qd() << "triangle  event enter " << position();
}

void QuickItemTriangle::hoverLeaveEvent(QHoverEvent* event)
{
    qd() << "triangle event leave" << position();
}
