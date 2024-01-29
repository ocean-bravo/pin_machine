#include "scan_view.h"
#include "ui_scan_view.h"

#include "wait.h"
#include "utils2.h"
#include "common.h"
#include "data_bus.h"

#include "scene.h"
#include "blob_item.h"
#include "task_scan_position.h"
#include "task_best_path.h"


#include <QMessageBox>
#include <QGraphicsScene>
#include <QGraphicsEllipseItem>
#include <QLabel>
#include <QJsonObject>

#include <functional>

#include "camera_view_item.h"

namespace {

// Не понимаю, что я тут делаю - как лямбда передается в функцию. Но работает.
template<typename T>
void databusAction(const QString& dbkey, T&& func)
{
    qd() << "db action";
    QObject::connect(&db(), &DataBus::valueChanged, [func = std::move(func), dbkey](const QString& key, const QVariant&)
    {
        if (key == dbkey)
            func();
    });
}

template<typename T>
void databusAction2(const QString& dbkey, T&& func)
{
    qd() << "db action";
    QObject::connect(&db(), &DataBus::valueChanged, [func = std::move(func), dbkey](const QString& key, const QVariant& value)
    {
        if (key == dbkey)
            func(value);
    });
}

}

ScanView::ScanView(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ScanView)
{
    ui->setupUi(this);

    _scene = &scene();

    ui->graphicsView->setScene(_scene);
    TaskScanPosition* taskScanPosition = new TaskScanPosition(this);
    TaskBestPath* taskBestPath = new TaskBestPath(this);

    connect(ui->graphicsView, &GraphicsView::scanPosition, taskScanPosition, &TaskScanPosition::run);

    connect(ui->graphicsView, &GraphicsView::addBlob, this, [this](QPointF pos)
    {
        scene().addBlob(pos.x(), pos.y(), 1.0);
    });

    connect(ui->graphicsView, &GraphicsView::calcPath, this, [=](QPointF pos)
    {
        if (taskBestPath->isRunning())
        {
            taskBestPath->stopProgram();
            waitForSignal(taskBestPath, &TaskBestPath::finished, 10000);
        }
        taskBestPath->run(pos);
    });

    databusAction("xPos", [this]() { updateCameraView(); });
    databusAction("yPos", [this]() { updateCameraView(); } );

    databusAction("best_path_stop", [taskBestPath]() { if (taskBestPath->isRunning()) taskBestPath->stopProgram(); } );

    databusAction2("best_path_optimized", [](const QVariant& value)
    {
        QList<QPointF> path = value.value<QList<QPointF>>();
        scene().drawPath(path);
    } );


    QLabel* message1 = new QLabel;
    message1->setFixedWidth(150);
    message1->setStyleSheet("QLabel { background-color : lightgrey; }");
    ui->statusBar->addWidget(message1);

    QLabel* message2 = new QLabel;
    message2->setFixedWidth(150);
    message2->setStyleSheet("QLabel { background-color : lightgrey; }");
    ui->statusBar->addWidget(message2);

    QLabel* message3 = new QLabel;
    message3->setFixedWidth(150);
    message3->setStyleSheet("QLabel { background-color : lightgrey; }");
    ui->statusBar->addWidget(message3);

    QLabel* message4 = new QLabel;
    message4->setFixedWidth(150);
    message4->setStyleSheet("QLabel { background-color : lightgrey; }");
    ui->statusBar->addWidget(message4);

    QLabel* message5 = new QLabel;
    message5->setFixedWidth(150);
    message5->setStyleSheet("QLabel { background-color : lightgrey; }");
    ui->statusBar->addWidget(message5);

    QLabel* message6 = new QLabel;
    message6->setFixedWidth(150);
    message6->setStyleSheet("QLabel { background-color : lightgrey; }");
    ui->statusBar->addWidget(message6);

    connect(&db(), &DataBus::valueChanged, this, [=](const QString& key, const QVariant& value)
    {
        if (key != "message")
            return;

        const QJsonObject msg = value.toJsonObject();

        const int labelNumber = msg.value("label_number").toInt();
        const QString text = msg.value("text").toString();

        if (labelNumber == 0)
            message1->setText(text);
        else if (labelNumber == 1)
            message2->setText(text);
        else if (labelNumber == 2)
            message3->setText(text);
        else if (labelNumber == 3)
            message4->setText(text);
        else if (labelNumber == 4)
            message5->setText(text);
        else if (labelNumber == 5)
            message6->setText(text);
    });
}

ScanView::~ScanView()
{
    delete ui;
}

void ScanView::updateCameraView()
{
    double x = db().value("xPos").toDouble();
    double y = db().value("yPos").toDouble();

    double w = db().value("resolution_width").toInt();
    double h = db().value("resolution_height").toInt();
    double ps = db().pixInMm();

    for (QGraphicsItem* item : _scene->items())
    {
        if (is<CameraViewItem>(item))
        {
            CameraViewItem* rect = dynamic_cast<CameraViewItem*>(item);
            rect->setRect(-w/(ps*2), -h/(ps*2), w/ps, h/ps);
            item->setPos(x, y);
            return;
        }
    }
}
