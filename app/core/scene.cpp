#include "scene.h"

#include "blob_item.h"
#include "camera_view_item.h"
#include "data_bus.h"
#include "utils2.h"
#include "common.h"


Scene::Scene(QObject* parent)
    : QGraphicsScene(parent)
{

}

Scene::~Scene()
{

}

void Scene::addBlob(double x, double y, double dia)
{
    //BlobItem* blob = new BlobItem(x, y, dia);//addEllipse(-dia/2, -dia/2, dia, dia, redPen);
    runOnThread(this, [this, x, y, dia]() { addItem(new BlobItem(x, y, dia)); });

    //item->setPos(x, y);
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

    runOnThread(this, [this, item]() { addItem(item); });
}

void Scene::removeDuplicatedBlobs()
{
    // если есть пересечение с кем то, то удалить его
    for (QGraphicsItem* item : items())
    {
        if (isNot<BlobItem>(item))
            continue;

        for (QGraphicsItem* collidingItem : collidingItems(item))
        {
            if (is<BlobItem>(collidingItem))
            {
                delete item;
                break;
            }
        }
    }
}
