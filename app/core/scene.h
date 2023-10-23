#pragma once

#include <QDialog>
#include <QGraphicsItem>

class QGraphicsScene;

namespace Ui { class Scene; }


class Scene : public QWidget
{
    Q_OBJECT

public:
    Scene(QWidget* parent = nullptr);
    ~Scene();

protected slots:

protected:


    Ui::Scene* ui;
    QGraphicsScene*  _scene;
};
