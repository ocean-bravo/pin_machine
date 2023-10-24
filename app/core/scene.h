#pragma once

#include <QWidget>

class QGraphicsScene;

namespace Ui { class Scene; }

class Scene : public QWidget
{
    Q_OBJECT

public:
    Scene(QWidget* parent = nullptr);
    ~Scene();

protected slots:


private:

    void setCross();
    Ui::Scene* ui;
    QGraphicsScene*  _scene;
};
