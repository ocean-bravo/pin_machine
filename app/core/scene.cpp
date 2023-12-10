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
#include <QUrl>

#include <QBuffer>

Scene::Scene(QObject* parent)
    : QGraphicsScene(-1000, -1000, 2000, 2000, parent) // Чтобы плату можно было двигать за пределы видимости
{
    connect(&db(), &DataBus::pixelSizeChanged, this, [this]()
    {
        every<QGraphicsPixmapItem>(items(Qt::AscendingOrder), [this](QGraphicsPixmapItem* pixmap)
        {
           auto pix = pixmap->pixmap();

           pix.setDevicePixelRatio(db().pixInMm());
           pixmap->setPixmap(pix);
        });
    });
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
    Q_UNUSED(angle)
    Q_UNUSED(distance)

}

void Scene::setImage(QImage img)
{
    QMetaObject::invokeMethod(this, "setImagePrivate", Qt::QueuedConnection, Q_ARG(QImage, img));
}

void Scene::setImagePrivate(QImage img)
{
    const double x = img.text("x").toDouble();
    const double y = img.text("y").toDouble();

    const double pixelSize = db().pixInMm();

    // Изображение нужно перевернуть по вертикали, т.к. сцена перевернута
    img = img.mirrored(false, true); // тут копия img

    QPixmap pix = QPixmap::fromImage(img);
    pix.setDevicePixelRatio(pixelSize);

    if (!_board)
        addBoard();

    QGraphicsPixmapItem* item = new QGraphicsPixmapItem(pix, _board);

    // Сдвиг на половину размера изображения, т.к. x и y - это координаты центра изображения
    item->setOffset(-pix.rect().width() / 2, -pix.rect().height() / 2);
    item->setPos(x, y);
    item->setZValue(-1); // Чтобы изображения были позади блобов
}

void Scene::saveScene(const QString& url)
{
    qd() << "save scene begin";

    ScopedMeasure sm("save scene end");

    Measure mes("get pixmap");

    QVariantMap map;

    int i = 0;
    // Порядок items важен. Именно так выглядит как сканировалось.
    every<QGraphicsPixmapItem>(items(Qt::AscendingOrder), [&map, &i, this](QGraphicsPixmapItem* pixmap)
    {
        const QString mainKey = "background_" + toInt(i);
        ++i;

        QImage img = pixmap->pixmap().toImage();
        QByteArray ba(reinterpret_cast<const char *>(img.constBits()), img.sizeInBytes());
//        QBuffer buffer(&ba);
//        buffer.open(QIODevice::WriteOnly);
//        img.save(&buffer, "PNG");

        map.insert(mainKey, QVariant()); // Для удобства поиска, пустая запись
        map.insert(mainKey + ".img" , qCompress(ba, 1)); // Уровень компрессии достаточный
        map.insert(mainKey + ".img.width", img.width());
        map.insert(mainKey + ".img.height", img.height());
        map.insert(mainKey + ".offset" , pixmap->offset());
        map.insert(mainKey + ".scale" , pixmap->scale());
        map.insert(mainKey + ".pos" , pixmap->pos());
        map.insert(mainKey + ".zValue" , pixmap->zValue());
        emit imageSaved(i);
    });

    mes.stop();

    i = 0;
    every<BlobItem>(items(), [&map, &i, this](BlobItem* blob)
    {
        const QString mainKey = "blob" + toInt(i);
        ++i;

        map.insert(mainKey, QVariant()); // Для удобства поиска, пустая запись
        map.insert(mainKey + ".pos" , blob->pos());
        map.insert(mainKey + ".dia" , blob->rect().width());
        map.insert(mainKey + ".isFiducial" , blob->isFiducial());
        map.insert(mainKey + ".isPunch" , blob->isPunch());
    });

    Measure mes2("datastream");
    QByteArray ba;
    QDataStream out(&ba, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_15);
    out << map;
    mes2.stop();


    Measure mes3("safetofile");

    QFile file(QUrl(url).toLocalFile());

    if (!file.open(QFile::WriteOnly))
    {
        qd() << "couldnt open file for save: " << QUrl(url).toLocalFile();
        return;
    }

    file.write(ba);

    mes3.stop();
}

void Scene::loadScene(const QString& url)
{
    clear();
    addBoard();

    QFile file(QUrl(url).toLocalFile());

    if (!file.exists())
    {
        qd() << "file not exists: " << QUrl(url).toLocalFile();
        return;
    }

    if (!file.open(QFile::ReadOnly))
    {
        qd() << "cant opent file: " << QUrl(url).toLocalFile();
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

        QByteArray ba = map.value(mainKey + ".img").toByteArray();
        int imgWidth = map.value(mainKey + ".img.width").toInt();
        int imgHeight = map.value(mainKey + ".img.height").toInt();
        QPointF offset = map.value(mainKey + ".offset").toPointF();
        double scale= map.value(mainKey + ".scale").toDouble();
        QPointF pos = map.value(mainKey + ".pos").toPointF();
        double zValue = map.value(mainKey + ".zValue").toDouble();

        ba = qUncompress(ba);

        // img и ba располалагают 1 буфером. Владеет им ba.
        QImage img(reinterpret_cast<const uchar *>(ba.constData()), imgWidth, imgHeight, QImage::Format_RGB32); // Такой формат у сохраняемого изображения.

        img = img.copy(); // Копия нужна. Теперь у img свой буфер, не зависимый от ba. Когда ba удалится, img будет жить.

        QGraphicsPixmapItem* item = new QGraphicsPixmapItem(QPixmap::fromImage(std::move(img)), _board);

        // Сдвиг на половину размера изображения, т.к. x и y - это координаты центра изображения
        item->setOffset(offset);
        item->setScale(scale);
        item->setPos(pos);
        item->setZValue(zValue); // Чтобы изображения были позади блобов
    }

    i = 0;
    while (true)
    {
        const QString mainKey = "blob" + toInt(i);
        ++i;

        if (!map.contains(mainKey))
            break;

        const QPointF pos = map.value(mainKey + ".pos").toPointF();
        const double dia = map.value(mainKey + ".dia").toDouble();
        const bool isFiducial = map.value(mainKey + ".isFiducial").toBool();
        const bool isPunch = map.value(mainKey + ".isPunch").toBool();

        BlobItem* blob = addBlob(pos.x(), pos.y(), dia);
        blob->setPunch(isPunch);
        blob->setFiducial(isFiducial);
    }
}

int Scene::images() const
{
    int i = 0;
    every<QGraphicsPixmapItem>(items(), [&i](QGraphicsPixmapItem*) { ++i; });
    return i;
}

void Scene::removeDuplicatedBlobs()
{
    //QMutexLocker locker(&_mutex);

    auto foo = [this]()
    {
        every<BlobItem>(items(), [this](BlobItem* blob)
        {
            // если есть пересечение с кем то, то удалить его
            const auto collidingItems = QGraphicsScene::collidingItems(blob);
            for (QGraphicsItem* collidingItem : collidingItems)
            {
                if (is<BlobItem>(collidingItem))
                {
                    delete blob;
                    break;
                }
            }
        });
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
