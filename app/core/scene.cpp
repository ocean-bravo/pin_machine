#include "scene.h"
#include "ui_scene.h"
#include "utils2.h"
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

//    QPen pen(Qt::green, 31, Qt::SolidLine);
//    _scene->addRect(0, 0, 300, 300, pen);

    connect(&db(), &DataBus::valueChanged, this, [this](const QString& key, const QVariant& value)
    {
        if (key != "found_blobs3")
            return;

        QPen greenPen(Qt::green, 1, Qt::SolidLine);
        QPen redPen(Qt::red, 0, Qt::SolidLine);

        _scene->clear();
        _scene->addRect(0, 0, 300, 300, greenPen);

        // Отправляю все блобы на сцену
        const QStringList blobs = value.toStringList();
        for (const QString& blob : blobs)
        {
            auto [x, y, dia] = blobToDouble(blob);
            QGraphicsEllipseItem* item = _scene->addEllipse(-dia/2, -dia/2, dia, dia, redPen);
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
