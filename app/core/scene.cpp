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

#include <QScopeGuard>
#include <QTimer>
#include <QJsonObject>

#include <QEvent>
#include <QMetaMethod>

#include <QtCore/private/qobject_p.h>

Scene::Scene(QObject* parent)
    : QGraphicsScene(-500, -500, 1000, 1000, parent) // Чтобы плату можно было двигать за пределы видимости
{
    //    connect(&db(), &DataBus::pixelSizeChanged, this, [this]()
    //    {
    //        every<QGraphicsPixmapItem>(items(Qt::AscendingOrder), [this](QGraphicsPixmapItem* pixmap)
    //        {
    //            auto pix = pixmap->pixmap();
    //            //auto pos = pixmap->pos();

    //            double pixInMm = db().pixInMm();
    //            pix.setDevicePixelRatio(pixInMm);
    //            pixmap->setPixmap(pix);


    //            pixmap->setOffset(-pix.rect().width() / (2*pixInMm), -pix.rect().height() / (2*pixInMm));
    //        });
    //    });


    _board = new BoardItem;
    addItem(_board);

    // Для снижения нагрузки на процессор, чуть разряжаю запросы на рисовку пути.
    // Если много свалилось запросов, выкидываю все, кроме последнего. Его и рисую.
    _drawPathTimer = new QTimer(this);
    _drawPathTimer->setSingleShot(true);
    _drawPathTimer->setInterval(50);

    connect(_drawPathTimer, &QTimer::timeout, this, [this]()
    {
        if (_pathQueue.empty())
            return;

        QList<QPointF> path = _pathQueue.front();

        every<QGraphicsLineItem>(QGraphicsScene::items(), [](QGraphicsLineItem* line) { delete line; });

        if (path.empty())
        {
            _pathQueue.removeFirst();
            _drawPathTimer->start();
            return;
        }

        double length = 0;
        for (int i = 0; i < path.size() - 1; ++i)
        {
            auto lineItem = addLine(QLineF(path.at(i), path.at(i+1)), QPen(Qt::red, 0.5));
            length += lineItem->line().length();
        }

        QJsonObject jo;
        jo.insert("label_number", 4);
        jo.insert("text", QString("length: %1").arg(toReal2(length)));
        db().insert("message", jo);

        _pathQueue.removeFirst();
        _drawPathTimer->start();
    }, Qt::QueuedConnection);
}

Scene::~Scene()
{

}

BlobItem* Scene::addBlob(double x, double y, double dia, bool sceneIsParent)
{
    //qd() << "add blob " << x << y << dia;

    if (x == 0)
        asm("nop");

    BlobItem* blob = new BlobItem(x, y, dia);
    blob->setHighlight(db().value("blobs_highlight").toBool());

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
    QString sceneFilename = blob->sceneFileName();
    auto newBlob = addBlob(x, y, dia, sceneIsParent);
    newBlob->setSceneFileName(sceneFilename);
    return newBlob;
}

void Scene::addBoard()
{
    //QMutexLocker locker(&_mutex);
    //_board.reset(new BoardItem);

    //    runOnThread(this, [this]() { addItem(_board.data()); });
    //    runOnThread(this, [this]() { addItem(new CameraViewItem); });

    runOnThreadWait(this, [this]()
    {
        every<BoardItem>(QGraphicsScene::items(), [](BoardItem* board) { delete board; });
        _board = new BoardItem;
        addItem(_board);
        every<CameraViewItem>(QGraphicsScene::items(), [](CameraViewItem* camera) { delete camera; });
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
    // Изображение нужно перевернуть по вертикали, т.к. сцена перевернута
    img = std::move(img.mirrored(false, true));

    //qd() << "img format befor pixmap " << img.format();
    QPixmap pix = QPixmap::fromImage(img);

    //pix.setDevicePixelRatio(pixInMm);

    if (!_board)
        addBoard();

    QGraphicsPixmapItem* item = new QGraphicsPixmapItem(pix, _board);

    //    qd() << "pix rect " << pix.rect();
    // Сдвиг на половину размера изображения, т.к. x и y - это координаты центра изображения
    const double pixInMm = img.devicePixelRatioF();
    item->setOffset(-img.width() / (2*pixInMm), -img.height() / (2*pixInMm));
    const double x = img.text("x").toDouble();
    const double y = img.text("y").toDouble();
    item->setPos(x, y);
    item->setZValue(-1); // Чтобы изображения были позади блобов

    static int i = 0;
    qd() << "set image i x y pos offset " << i << x << y << item->pos() << item->offset();
    //        qd() << "pix rect " << pix.rect();
    //            qd() << "pix offset " << item->offset();
}

QList<QGraphicsPixmapItem*> Scene::pixmaps() const
{
    QList<QGraphicsPixmapItem*> pix;
    // Порядок items важен. Именно так выглядит как сканировалось.
    every<QGraphicsPixmapItem>(items(Qt::AscendingOrder), [&pix](QGraphicsPixmapItem* pixmap)
    {
        pix.append(pixmap);
    });
    return pix;
}

void Scene::saveScene(const QString& url)
{
    QByteArray ba = saveSceneToByteArray();

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
        qd() << "file not exists: " << url << QUrl(url).toLocalFile();
        return;
    }

    if (!file.open(QFile::ReadOnly))
    {
        qd() << "cant opent file: " << url << QUrl(url).toLocalFile();
        return;
    }

    const QByteArray ba = file.readAll();
    file.close();

    loadSceneFromByteArray(ba);
}

QByteArray Scene::saveSceneToByteArray()
{
    qd() << "save scene begin";

    ScopedMeasure sm("save scene end");

    Measure mes("get pixmap");

    QVariantMap map;

    QList<QGraphicsPixmapItem*> pixs = pixmaps();

    for (int i = 0; i < pixs.size(); ++i)
    {
        QGraphicsPixmapItem* pixmap = pixs[i];

        const QString mainKey = "bg." + toInt(i);

        QImage img = pixmap->pixmap().toImage(); // format получается Format_RGB32
        img = std::move(img.mirrored(false, true)); // надо повернуть img, т.к. при загрузке img будет повернуто снова
        img.convertTo(QImage::Format_RGB888, Qt::ColorOnly);

        //qd() << " img formata in save " << img.format();
        QByteArray ba(reinterpret_cast<const char *>(img.constBits()), img.sizeInBytes());

        map.insert(mainKey, QVariant()); // Для удобства поиска, пустая запись
        map.insert(mainKey + ".img" , qCompress(ba, 1)); // Уровень компрессии достаточный
        map.insert(mainKey + ".width", img.width());
        map.insert(mainKey + ".height", img.height());
        //map.insert(mainKey + ".devicePixelRatio", pixmap->pixmap().devicePixelRatio());
        map.insert(mainKey + ".devicePixelRatio", img.devicePixelRatioF());

        map.insert(mainKey + ".offset" , pixmap->offset());
        map.insert(mainKey + ".scale" , pixmap->scale());
        map.insert(mainKey + ".pos.x" , img.text("x"));
        map.insert(mainKey + ".pos.y" , img.text("y"));
        map.insert(mainKey + ".zValue" , pixmap->zValue());

        //qd() << "save image i x y pos offset " << i << img.text("x") << img.text("y") << pixmap->pos() << pixmap->offset();
        emit imageSaved(i+1);
    }

    mes.stop();

    int i = 0;
    every<BlobItem>(items(), [&map, &i, this](BlobItem* blob)
    {
        const QString mainKey = "blob." + toInt(i++);

        map.insert(mainKey, QVariant()); // Для удобства поиска, пустая запись
        map.insert(mainKey + ".pos" , blob->pos());
        map.insert(mainKey + ".dia" , blob->rect().width());
        map.insert(mainKey + ".isFiducial" , blob->isFiducial());
        map.insert(mainKey + ".isPunch" , blob->isPunch());
        map.insert(mainKey + ".sceneFilename" , blob->sceneFileName());
    });

    Measure mes2("datastream");
    QByteArray ba;

    QDataStream out(&ba, QIODevice::WriteOnly);

    out.setVersion(QDataStream::Qt_5_15);
    out << map;
    mes2.stop();

    return ba;
}

void Scene::loadSceneFromByteArray(const QByteArray& ba)
{
    QVariantMap map;
    QDataStream in(ba);
    in.setVersion(QDataStream::Qt_5_15);
    in >> map;

    int i = 0;

    while (true)
    {
        const QString mainKey = "bg." + toInt(i++);

        if (!map.contains(mainKey))
            break;

        QByteArray ba = map.value(mainKey + ".img").toByteArray();
        int imgWidth = map.value(mainKey + ".width").toInt();
        int imgHeight = map.value(mainKey + ".height").toInt();
        double devicePixelRatio = map.value(mainKey + ".devicePixelRatio").toDouble();

        QPointF offset = map.value(mainKey + ".offset").toPointF();
        double scale = map.value(mainKey + ".scale").toDouble();
        QString x = map.value(mainKey + ".pos.x").toString();
        QString y = map.value(mainKey + ".pos.y").toString();
        double zValue = map.value(mainKey + ".zValue").toDouble();

        ba = qUncompress(ba);

        // img и ba располалагают 1 буфером. Владеет им ba.
        QImage img(reinterpret_cast<const uchar *>(ba.constData()), imgWidth, imgHeight, QImage::Format_RGB888); // Такой формат у сохраняемого изображения.
        img.setDevicePixelRatio(devicePixelRatio);
        img.setText("x", x);
        img.setText("y", y);

        setImagePrivate(img); // Внутри делается копия. Копия нужна. Теперь у img свой буфер, не зависимый от ba. Когда ba удалится, img будет жить.

        // img = img.copy();

        // QPixmap pix = std::move(QPixmap::fromImage(std::move(img)));
        // //pix.setDevicePixelRatio(devicePixelRatio);

        // QGraphicsPixmapItem* item = new QGraphicsPixmapItem(pix, _board);

        // // Сдвиг на половину размера изображения, т.к. x и y - это координаты центра изображения
        // item->setOffset(offset);
        // item->setScale(scale);
        // item->setPos(QPointF(x.toDouble(), y.toDouble()));
        // item->setZValue(zValue); // Чтобы изображения были позади блобов

        // //qd() << "load image i x y pos offset " << i << img.text("x") << img.text("y") << item->pos() << item->offset();
        // ++i;
    }

    i = 0;
    while (true)
    {
        const QString mainKey = "blob." + toInt(i++);

        if (!map.contains(mainKey))
            break;

        const QPointF pos = map.value(mainKey + ".pos").toPointF();
        const double dia = map.value(mainKey + ".dia").toDouble();
        const bool isFiducial = map.value(mainKey + ".isFiducial").toBool();
        const bool isPunch = map.value(mainKey + ".isPunch").toBool();
        const QString sceneFilename = map.value(mainKey + ".sceneFilename").toString();

        BlobItem* blob = addBlob(pos.x(), pos.y(), dia);
        blob->setPunch(isPunch);
        blob->setFiducial(isFiducial);
        blob->setSceneFileName(sceneFilename);
    }
}

void Scene::highlightBlobs(bool state)
{
    every<BlobItem>(QGraphicsScene::items(), [this, state](BlobItem* blob) { blob->setHighlight(state); });
}

void Scene::setSelectedAsPunch(bool state)
{
    every<BlobItem>(QGraphicsScene::items(), [this, state](BlobItem* blob)
    {
        if (blob->isSelected())
        {
            blob->setPunch(state);
            blob->setSelected(false);
        }
    });
}

int Scene::images() const
{
    int i = 0;
    every<QGraphicsPixmapItem>(items(), [&i](QGraphicsPixmapItem*) { ++i; });
    return i;
}

QList<BlobItem*> Scene::punchBlobs()
{
    QList<BlobItem*> blobs;
    blobs.reserve(300);

    runOnThreadWait(this, [this, &blobs]()
    {
        every<BlobItem>(QGraphicsScene::items(), [&blobs](BlobItem* blob)
        {
            if (blob->isPunch())
                blobs.push_back(blob);
        });
    });

    return blobs;
}

void Scene::deleteSelectedBlobs()
{
    every<BlobItem>(QGraphicsScene::items(), [this](BlobItem* blob)
    {
        if (blob->isSelected())
            blob->deleteLater();
    });
}

void Scene::selectAllBlobs()
{
    every<BlobItem>(QGraphicsScene::items(), [this](BlobItem* blob)
    {
        blob->setSelected(true);
    });
}

void Scene::drawPath(const QList<QPointF>& path)
{
    runOnThread(this, [this, path]()
    {
        _pathQueue.clear();
        _pathQueue.append(path);
        _drawPathTimer->start();
    });
}

bool Scene::event(QEvent* event)
{
    // if (event->type() != QEvent::GraphicsSceneMouseMove)
    //     qd() << "scene event " << event->type();

    ///qd() << "scene event " << event->type();
    if (event->type() == QEvent::GraphicsSceneMouseMove)
    {
        // QGraphicsSceneEvent* ev = static_cast<QGraphicsSceneEvent* >(event);
        // qd() << ev;
        asm("nop");
    }

    if (event->type() == QEvent::MetaCall)
    {
        ///qd() << "scene event " << event;

        QMetaCallEvent * mev = static_cast<QMetaCallEvent*>(event);

         QMetaMethod slot = this->metaObject()->method(mev->id());
        /// qd() << "Metacall:" << slot.methodSignature();
    }


    QGraphicsScene::event(event);

}

void Scene::removeDuplicatedBlobs()
{
    //QMutexLocker locker(&_mutex);

    auto foo = [this]()
    {
        every<BlobItem>(QGraphicsScene::items(Qt::AscendingOrder), [this](BlobItem* blob)
        {
            // если есть пересечение с кем то, то удалить его
            const QList<QGraphicsItem*> collidingItems = QGraphicsScene::collidingItems(blob, Qt::IntersectsItemShape);
            const QList<BlobItem*> collidingBlobs = convertTo<BlobItem>(collidingItems);

            if (collidingBlobs.isEmpty())
                return;

            // Перенос признаков от удаляемого блоба к новым блобам. Если старый был punch, надо чтобы этот признак не потерялся.
            for (BlobItem* collidingBlob : collidingBlobs)
            {
                collidingBlob->setFiducial(blob->isFiducial());
                collidingBlob->setPunch(blob->isPunch());
            }

            delete blob;
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

void Scene::updateBlob(BlobItem* blob, double sceneX, double sceneY, double dia)
{
    //QMutexLocker locker(&_mutex);

    auto foo = [blob, sceneX, sceneY, dia]()
    {
        blob->setPos(blob->mapToParent(blob->mapFromScene(sceneX, sceneY)));
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
    QList<QGraphicsItem*> items;
    auto foo = [this, order, &items]()
    {
        items = QGraphicsScene::items(order);
    };
    runOnThreadWait((QObject*)this, foo);
    return items;
}
