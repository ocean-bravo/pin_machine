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
    //QMutexLocker locker(&_mutex);

    auto foo = [this](double x, double y, double dia)
    {
        addItem(new BlobItem(x, y, dia));
    };

    static const QThread* sceneThread = thread();
    const QThread* executorThread = QThread::currentThread();

    if (sceneThread != executorThread)
    {
        QEventLoop loop;
        runOnThread(this, [this, foo, x, y, dia, &loop]()
        {
            foo(x, y, dia);
            loop.quit();
        });

        loop.exec();
    }
    else
    {
        runOnThread(this, [this, foo, x, y, dia]()
        {
            foo(x, y, dia);
        });
    }
}

void Scene::addBorder()
{
    static const QPen greenPen(Qt::green, 1, Qt::SolidLine);

    //QMutexLocker locker(&_mutex);

    runOnThread(this, [this]() { addRect(0, 0, 300, 300, greenPen); });
    runOnThread(this, [this]() { addItem(new CameraViewItem); });
}

void Scene::setImage(QImage img)
{
    //QMutexLocker locker(&_mutex);

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

//    auto foo = [this](QGraphicsPixmapItem* item)
//    {
        addItem(item);
//    };

//    static const QThread* sceneThread = thread();
//    const QThread* executorThread = QThread::currentThread();

//    if (sceneThread != executorThread)
//    {
//        QEventLoop loop;
//        runOnThread(this, [this, &foo, item, &loop]()
//        {
//            foo(item);
//            loop.quit();
//        });

//        loop.exec();
//    }
//    else
//    {
//        runOnThread(this, [this, &foo, item]()
//        {
//            foo(item);
//        });
//    }
}

void Scene::removeDuplicatedBlobs()
{
    //QMutexLocker locker(&_mutex);

    auto foo = [this]()
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
    };

    static const QThread* sceneThread = thread();
    const QThread* executorThread = QThread::currentThread();

    if (sceneThread != executorThread)
    {
        QEventLoop loop;
        runOnThread(this, [this, &foo, &loop]()
        {
            foo();
            loop.quit();
        });

        loop.exec();
    }
    else
    {
        runOnThread(this, [this, &foo]()
        {
            foo();
        });
    }
}

void Scene::updateBlob(BlobItem* blob, double x, double y, double dia)
{
    //QMutexLocker locker(&_mutex);

    auto foo = [](BlobItem* blob, double x, double y, double dia)
    {
        blob->setX(x);
        blob->setY(y);
        blob->setRect(-dia/2, -dia/2, dia, dia);
    };

    static const QThread* sceneThread = thread();
    const QThread* executorThread = QThread::currentThread();

    if (sceneThread != executorThread)
    {
        QEventLoop loop;
        runOnThread(this, [this, &foo, blob, x, y, dia, &loop]()
        {
            foo(blob, x, y, dia);
            loop.quit();
        });

        loop.exec();
    }
    else
    {
        runOnThread(this, [this, &foo, blob, x, y, dia]()
        {
            foo(blob, x, y, dia);
        });
    }
}

QList<QGraphicsItem*> Scene::items(Qt::SortOrder order) const
{
    return QGraphicsScene::items(order);
}
