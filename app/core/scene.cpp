#include "scene.h"
#include "ui_scene.h"

#include "data_bus.h"

#include <QMessageBox>
#include <QGraphicsScene>
#include <QTimer>

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

    QPen pen(Qt::green, 31, Qt::SolidLine);
    _scene->addRect(0, 0, 300, 300, pen);


    QTimer* timer = new QTimer(this);
    timer->start(100);
    connect(timer, &QTimer::timeout, this, [this]()
    {
//        if (key != "found_blobs3")
//            return;

        ScopedMeasure ms("add to scene");
        QStringList blobs = db().value("found_blobs3").toStringList();

        _scene->clear();
        QPen pen(Qt::green, 1, Qt::SolidLine);
        _scene->addRect(0, 0, 300, 300, pen);

        // Отправляю все блобы на сцену
        for (const QString& blob : blobs)
        {
            QStringList coord = blob.split(" ", Qt::SkipEmptyParts);
            qd() << coord;
            double x = coord[0].toDouble();
            double y = coord[1].toDouble();
            double dia = coord[2].toDouble();

            QPen pen(Qt::red, 0, Qt::SolidLine);
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
