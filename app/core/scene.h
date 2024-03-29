#pragma once

#include <QGraphicsScene>
#include <QMetaObject>
#include <QMutex>

#include "singleton.h"

class BoardItem;
class BlobItem;
class QGraphicsScene;

class Scene : public QGraphicsScene, public Singleton<Scene>
{
    Q_OBJECT

public:
    BlobItem* addBlob(double x, double y, double dia, bool sceneIsParent = false);
    BlobItem* addBlobCopy(const BlobItem* blob, bool sceneIsParent = false);

    void addBoard();
    QGraphicsItem* board() const;
    void moveBoard(double angle, double distance);

    void removeDuplicatedBlobs();
    void updateBlob(BlobItem* blob, double x, double y, double dia);

    QList<QGraphicsItem*> items(Qt::SortOrder order = Qt::DescendingOrder) const;

    // Координаты изображения идут вместе с изображением
    void setImage(QImage img);

    void saveScene(const QString &url);
    void loadScene(const QString &url);

    int images() const;

signals:
    void imageSaved(int);

private slots:
    void setImagePrivate(QImage img);

private:
    Scene(QObject* parent = nullptr);
    ~Scene();

    QGraphicsItem* _board = nullptr;

    mutable QMutex _mutex;

    friend class Singleton<Scene>;
};


inline Scene& scene()
{
    return Scene::instance();
}
