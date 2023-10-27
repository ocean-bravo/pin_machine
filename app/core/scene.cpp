#include "scene.h"

#include <QGraphicsEllipseItem>
#include "camera_view_item.h"
#include "data_bus.h"


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

void Scene::setImage(QImage img)
{
    double x = img.text("x").toDouble();
    double y = img.text("y").toDouble();
    int w = img.width();
    int h = img.height();
    double pixelSize = db().value("pixel_size").toDouble();


    const int width = 150;
    const int height = 200;


    QGraphicsPixmapItem* item = new QGraphicsPixmapItem(QPixmap::fromImage(img));
    //item->setScale(0.1);
    //item->setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable);
    item->setPos(x, y);
    addItem(item);
}
