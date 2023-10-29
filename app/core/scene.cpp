#include "scene.h"

#include "blob_item.h"
#include "camera_view_item.h"
#include "data_bus.h"
#include "utils2.h"
#include "common.h"

#include <QMutexLocker>
#include <QEventLoop>

Scene::Scene(QObject* parent)
    : QGraphicsScene(parent)
{

}

Scene::~Scene()
{

}

void Scene::addBlob(double x, double y, double dia)
{
    QMutexLocker locker(&_mutex);

    //BlobItem* blob = new BlobItem(x, y, dia);//addEllipse(-dia/2, -dia/2, dia, dia, redPen);
    QEventLoop loop;
    auto foo = [this, x, y, dia, &loop]()
    {
        addItem(new BlobItem(x, y, dia));
        loop.quit();
    };
    runOnThread(this, foo);
    loop.exec();

    //item->setPos(x, y);
}

void Scene::addBorder()
{
    static const QPen greenPen(Qt::green, 1, Qt::SolidLine);

    QMutexLocker locker(&_mutex);

    runOnThread(this, [this]() { addRect(0, 0, 300, 300, greenPen); });
    runOnThread(this, [this]() { addItem(new CameraViewItem); });
}

void Scene::setImage(QImage img)
{
    QMutexLocker locker(&_mutex);

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

    QEventLoop loop;
    runOnThread(this, [this, item, &loop]()
    {
        addItem(item);
        loop.quit();
    });
    loop.exec();
}

void Scene::removeDuplicatedBlobs()
{
    QMutexLocker locker(&_mutex);

    QEventLoop loop;
    auto foo = [this, &loop]()
    {
        // если есть пересечение с кем то, то удалить его
        const auto items = QGraphicsScene::items();
        for (QGraphicsItem* item : items)
        {
            if (isNot<BlobItem>(item))
                continue;

            const auto collidingItems = QGraphicsScene::collidingItems(item);
            for (QGraphicsItem* collidingItem : collidingItems)
            {
                if (is<BlobItem>(collidingItem))
                {
                    delete item;
                    break;
                }
            }
        }
        loop.quit();
    };
    runOnThread(this, foo);
    loop.exec();
}

void Scene::updateBlob(BlobItem* blob, double x, double y, double dia)
{
    QMutexLocker locker(&_mutex);

    QEventLoop loop;
    auto foo = [this, blob, x, y, dia, &loop]()
    {
        blob->setX(x);
        blob->setY(y);
        blob->setRect(-dia/2, -dia/2, dia, dia);
        loop.quit();
    };
    runOnThread(this, foo);
    loop.exec();
}

QList<QGraphicsItem *> Scene::items(Qt::SortOrder order) const
{
    QMutexLocker locker(&_mutex);
    return QGraphicsScene::items(order);
}
