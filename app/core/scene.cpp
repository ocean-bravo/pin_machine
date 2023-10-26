#include "scene.h"

#include <QGraphicsEllipseItem>

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
