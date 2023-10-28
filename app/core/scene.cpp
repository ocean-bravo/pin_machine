#include "scene.h"

#include <QGraphicsEllipseItem>
#include "camera_view_item.h"
#include "data_bus.h"

namespace {

template<typename Function>
auto runOnThread(QObject* targetObject, Function function)
{
    QMetaObject::invokeMethod(targetObject, std::move(function));
}

}


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
    static const QPen greenPen(Qt::green, 1, Qt::SolidLine);

    runOnThread(this, [this]() { addRect(0, 0, 300, 300, greenPen); });
    runOnThread(this, [this]() { addItem(new CameraViewItem); });
}

void Scene::setImage(QImage img)
{
    const double x = img.text("x").toDouble();
    const double y = img.text("y").toDouble();
    const int w = img.width();
    //const int h = img.height();
    const double pixelSize = db().value("pixel_size").toDouble();

    const double imageWidthMm = w * pixelSize;
    //const double imageHeightMm = h * pixelSize;

    // Изображение нужно перевернуть по вертикали, т.к. сцена перевернута
    img = img.mirrored(false, true); // тут копия img

    QPixmap pix = QPixmap::fromImage(img);

    const double ratio = pix.rect().width() / imageWidthMm;

    QGraphicsPixmapItem* item = new QGraphicsPixmapItem(pix);

    // Сдвиг на половину размера изображения, т.к. x и y - это координаты центра изображения
    item->setOffset(-pix.rect().width() / 2, -pix.rect().height() / 2);
    item->setScale(1/ratio);
    item->setPos(x, y);
    item->setZValue(-1); // Чтобы изображения были позади блобов

    runOnThread(this, [this, &item]() { addItem(item); });
}
