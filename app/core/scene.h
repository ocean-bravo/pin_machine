#pragma once

#include <QGraphicsScene>
#include <QMetaObject>
#include <QMutex>


#include "singleton.h"

class BlobItem;
class QGraphicsScene;

class Scene : public QGraphicsScene, public Singleton<Scene>
{
    Q_OBJECT

public:
    BlobItem* addBlob(double x, double y, double dia);
    BlobItem* addBlobCopy(const BlobItem* blob);

    void addBorder();
    // Координаты изображения идут вместе с изображением

    void removeDuplicatedBlobs();
    void updateBlob(BlobItem* blob, double x, double y, double dia);

    QList<QGraphicsItem *> items(Qt::SortOrder order = Qt::DescendingOrder) const;

    void setImage(QImage img);

private slots:
    void setImagePrivate(QImage img);

private:
    Scene(QObject* parent = nullptr);
    ~Scene();

    mutable QMutex _mutex;

    friend class Singleton<Scene>;
};


inline Scene& scene()
{
    return Scene::instance();
}
