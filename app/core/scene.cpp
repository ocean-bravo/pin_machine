#include "scene.h"

#include "blob_item.h"
#include "camera_view_item.h"
#include "board_item.h"

#include "data_bus.h"
#include "utils2.h"
#include "common.h"

#include <algorithm>

#include <QMutexLocker>
#include <QEventLoop>

#include <QFile>

#include <QBuffer>

Scene::Scene(QObject* parent)
    : QGraphicsScene(-1000, -1000, 2000, 2000, parent) // Чтобы плату можно было двигать за пределы видимости
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
            blob->setParentItem(_board);
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
    //_board.reset(new BoardItem);

    //    runOnThread(this, [this]() { addItem(_board.data()); });
    //    runOnThread(this, [this]() { addItem(new CameraViewItem); });

    runOnThreadWait(this, [this]()
    {
        every<BoardItem>(scene().items(), [](BoardItem* board) { delete board; });
        _board = new BoardItem;
        addItem(_board);
        every<CameraViewItem>(scene().items(), [](CameraViewItem* camera) { delete camera; });
        addItem(new CameraViewItem);
    });
}

QGraphicsItem* Scene::board() const
{
    return _board;
}

void Scene::moveBoard(double angle, double distance)
{


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

    QGraphicsPixmapItem* item = new QGraphicsPixmapItem(pix, _board);

    // Сдвиг на половину размера изображения, т.к. x и y - это координаты центра изображения
    item->setOffset(-pix.rect().width() / 2, -pix.rect().height() / 2);
    item->setScale(1/ratio);
    item->setPos(x, y);
    item->setZValue(-1); // Чтобы изображения были позади блобов

    //addItem(item);
}

void Scene::saveScene()
{
    Measure mes("get pixmap");

    QVariantMap map;

    int i = 0;
    every<QGraphicsPixmapItem>(items(), [&map, &i, this](QGraphicsPixmapItem* pixmap)
    {
        QImage img = pixmap->pixmap().toImage();
        const QPointF offset = pixmap->offset();
        const double scale = pixmap->scale();
        const QPointF pos = pixmap->pos();
        const double zValue = pixmap->zValue();

        const QString mainKey = "background_" + toInt(i);
        ++i;

        QByteArray ba;
        QBuffer buffer(&ba);
        buffer.open(QIODevice::WriteOnly);
        img.save(&buffer, "PPM");

        map.insert(mainKey, QVariant()); // Для удобства поиска, пустая запись
        map.insert(mainKey + ".img" , ba);
        map.insert(mainKey + ".offset" , offset);
        map.insert(mainKey + ".scale" , scale);
        map.insert(mainKey + ".pos" , pos);
        map.insert(mainKey + ".zValue" , zValue);
        emit imageSaved(i);
    });

    mes.stop();


    Measure mes2("datastream");
    QByteArray ba;
    QDataStream out(&ba, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_15);
    out << map;
    mes2.stop();


    Measure mes3("safetofile");
    saveDataToFile("", "scene_save", ba);
    mes3.stop();
}

void Scene::loadScene()
{
    clear();
    addBoard();

    QFile file("scene_save");

    if (!file.exists())
    {
        qd() << "file not exists";
        return;
    }

    if (!file.open(QFile::ReadOnly))
    {
        qd() << "cant opent file";
        return;
    }

    const QByteArray ba = file.readAll();
    file.close();

    QVariantMap map;
    QDataStream in(ba);
    in.setVersion(QDataStream::Qt_5_15);
    in >> map;

    int i = 0;

    while (true)
    {
        const QString mainKey = "background_" + toInt(i);
        ++i;

        if (!map.contains(mainKey))
            break;

        QImage img = map.value(mainKey + ".img").value<QImage>();
        QPointF offset = map.value(mainKey + ".offset").toPointF();
        double scale= map.value(mainKey + ".scale").toDouble();
        QPointF pos = map.value(mainKey + ".pos").toPointF();
        double zValue = map.value(mainKey + ".zValue").toDouble();

        QGraphicsPixmapItem* item = new QGraphicsPixmapItem(QPixmap::fromImage(std::move(img)), _board);

        // Сдвиг на половину размера изображения, т.к. x и y - это координаты центра изображения
        item->setOffset(offset);
        item->setScale(scale);
        item->setPos(pos);
        item->setZValue(zValue); // Чтобы изображения были позади блобов
    }
}

int Scene::images() const
{
    int i = 0;
    every<QGraphicsPixmapItem>(items(), [&i](QGraphicsPixmapItem*)
    {
        ++i;
    });
    return i;

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
