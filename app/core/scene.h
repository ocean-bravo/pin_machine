#pragma once

#include <QGraphicsScene>

#include "singleton.h"

class QGraphicsScene;

class Scene : public QGraphicsScene, public Singleton<Scene>
{
    Q_OBJECT

public:
    void addBlob(double x, double y, double dia);

    void addBorder();
    void setImage(QImage img);

private:
    Scene(QObject* parent = nullptr);
    ~Scene();

    friend class Singleton<Scene>;
};


inline Scene& scene()
{
    return Scene::instance();
}
