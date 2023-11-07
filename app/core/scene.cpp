#include "scene.h"

#include "blob_item.h"
#include "camera_view_item.h"
#include "board_item.h"

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

BlobItem* Scene::addBlob(double x, double y, double dia, bool sceneIsParent)
{
    //QMutexLocker locker(&_mutex);

    BlobItem* blob = new BlobItem(x, y, dia);

    auto foo = [this, blob, sceneIsParent]()
    {
        if (sceneIsParent)
            addItem(blob);
        else
            blob->setParentItem(_board.data());
    };

    runOnThreadWait(this, foo);

//    static const QThread* sceneThread = thread();
//    const QThread* executorThread = QThread::currentThread();

    //if (sceneThread != executorThread)
//    {
//        QEventLoop loop;
//        runOnThread(this, [this, foo, &loop]()
//        {
//            foo();
//            loop.quit();
//        });

//        loop.exec();
//    }
//    else
//    {
//        runOnThread(this, [this, foo]()
//        {
//            foo();
//        });
//    }
    return blob;
}

BlobItem* Scene::addBlobCopy(const BlobItem* blob, bool sceneIsParent)
{
    double x = blob->x();
    double y = blob->y();
    double dia = blob->rect().width();
    return addBlob(x, y, dia, sceneIsParent);
}

void Scene::addBoard()
{
   //QMutexLocker locker(&_mutex);
    _board.reset(new BoardItem);
//    runOnThread(this, [this]() { addItem(_board.data()); });
//    runOnThread(this, [this]() { addItem(new CameraViewItem); });

    runOnThreadWait(this, [this]() { addItem(_board.data()); });
    runOnThreadWait(this, [this]() { addItem(new CameraViewItem); });
}

QGraphicsItem* Scene::board() const
{
    return _board.data();
}

void Scene::setImage(QImage img)
{
    QMetaObject::invokeMethod(this, "setImagePrivate", Qt::QueuedConnection, Q_ARG(QImage, img));
}

void Scene::setImagePrivate(QImage img)
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

    QGraphicsPixmapItem* item = new QGraphicsPixmapItem(pix, _board.data());

    // Сдвиг на половину размера изображения, т.к. x и y - это координаты центра изображения
    item->setOffset(-pix.rect().width() / 2, -pix.rect().height() / 2);
    item->setScale(1/ratio);
    item->setPos(x, y);
    item->setZValue(-1); // Чтобы изображения были позади блобов

    //addItem(item);
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

    runOnThreadWait(this, foo);

//    static const QThread* sceneThread = thread();
//    const QThread* executorThread = QThread::currentThread();

    //if (sceneThread != executorThread)
//    {
//        QEventLoop loop;
//        runOnThread(this, [this, &foo, &loop]()
//        {
//            foo();
//            loop.quit();
//        });

//        loop.exec();
//    }
//    else
//    {
//        runOnThread(this, [this, &foo]()
//        {
//            foo();
//        });
//    }
}

void Scene::updateBlob(BlobItem* blob, double x, double y, double dia)
{
    //QMutexLocker locker(&_mutex);

    auto foo = [blob, x, y, dia]()
    {
        blob->setX(x);
        blob->setY(y);
        blob->setRect(-dia/2, -dia/2, dia, dia);
    };

    runOnThreadWait(this, foo);

//    static const QThread* sceneThread = thread();
//    const QThread* executorThread = QThread::currentThread();

    //if (sceneThread != executorThread)
//    {
//        QEventLoop loop;
//        runOnThread(this, [this, &foo, blob, x, y, dia, &loop]()
//        {
//            foo(blob, x, y, dia);
//            loop.quit();
//        });

//        loop.exec();
//    }
//    else
//    {
//        runOnThread(this, [this, &foo, blob, x, y, dia]()
//        {
//            foo(blob, x, y, dia);
//        });
//    }
}

QList<QGraphicsItem*> Scene::items(Qt::SortOrder order) const
{
    return QGraphicsScene::items(order);
}
