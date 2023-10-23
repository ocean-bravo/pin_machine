#include "scene.h"
#include "ui_scene.h"

#include "data_bus.h"

#include <QMessageBox>
#include <QGraphicsScene>

Scene::Scene(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Scene)
{
    ui->setupUi(this);
    // Только кнопки максимизации и закрытия. Убрал кнопку минимизации.
    //setWindowFlags(Qt::Window | Qt::WindowMaximizeButtonHint | Qt::WindowCloseButtonHint);
    setWindowModality(Qt::NonModal);

    _scene = new QGraphicsScene(this);

    ui->graphicsView->setScene(_scene);

    QPen pen(Qt::green, 3, Qt::SolidLine);
    _scene->addRect(0, 0, 300, 300, pen);

    connect(&db(), &DataBus::valueChanged, this, [this](const QString& key, const QVariant& blobs)
    {
        if (key != "found_blobs3")
            return;

        qd() <<  "blobs to scene";

        _scene->clear();
        QPen pen(Qt::green, 3, Qt::SolidLine);
        _scene->addRect(0, 0, 300, 300, pen);

        // Отправляю все блобы на сцену
        for (const QString& blob : blobs.toStringList())
        {
            QStringList coord = blob.split(" ");
            double x = coord[0].toDouble();
            double y = coord[1].toDouble();
            double dia = coord[3].toDouble();

            QPen pen(Qt::red, 3, Qt::SolidLine);
            QGraphicsEllipseItem* item = _scene->addEllipse(-dia/2, -dia/2, dia, dia, pen);
            item->setPos(x, y);
        }
    });
}

Scene::~Scene()
{
    delete ui;
}

//void Scene::fitMiniplateInView()
//{
//    ui->graphicsView->fit();
//}
