#include "scene.h"

#include <QGraphicsEllipseItem>
#include "camera_view_item.h"


Scene::Scene(QObject* parent)
    : QGraphicsScene(parent)
{

}

Scene::~Scene()
{

}

void Scene::addBlob(double x, double y, double dia)
{
    static const QPen redPen(Qt::red, 0, Qt::SolidLine);
    QGraphicsEllipseItem* item = addEllipse(-dia/2, -dia/2, dia, dia, redPen);
    item->setPos(x, y);
}

void Scene::addBorder()
{
    QPen greenPen(Qt::green, 1, Qt::SolidLine);
    addRect(0, 0, 300, 300, greenPen);
    addItem(new CameraViewItem);
}
