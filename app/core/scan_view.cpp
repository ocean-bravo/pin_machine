#include "scan_view.h"
#include "ui_scan_view.h"

#include "utils2.h"
#include "common.h"
#include "data_bus.h"

#include "scene.h"
#include <QMessageBox>
#include <QGraphicsScene>
#include <QGraphicsEllipseItem>

#include "camera_view_item.h"

ScanView::ScanView(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ScanView)
{
    ui->setupUi(this);

    _scene = &scene();

    ui->graphicsView->setScene(_scene);

    connect(&db(), &DataBus::valueChanged, this, [this](const QString& key, const QVariant& value)
    {
        if (key != "found_blobs3")
            return;

        QPen greenPen(Qt::green, 1, Qt::SolidLine);
        QPen redPen(Qt::red, 0, Qt::SolidLine);

        _scene->clear();
        _scene->addRect(0, 0, 300, 300, greenPen);
        _scene->addItem(new CameraViewItem);

        setCross();

        // Отправляю все блобы на сцену
        const QStringList blobs = value.toStringList();
        for (const QString& blob : blobs)
        {
            auto [x, y, dia] = blobToDouble(blob);
            QGraphicsEllipseItem* item = _scene->addEllipse(-dia/2, -dia/2, dia, dia, redPen);
            item->setPos(x, y);
        }
    });

    connect(&db(), &DataBus::valueChanged, this, [this](const QString& key, const QVariant&)
    {
        if (key == "xPos" || key == "yPos")
            setCross();
    });
}

ScanView::~ScanView()
{
    delete ui;
}

void ScanView::setCross()
{
    double x = db().value("xPos").toDouble();
    double y = db().value("yPos").toDouble();

    double w = db().value("resolution_width").toInt();
    double h = db().value("resolution_height").toInt();
    double ps = db().value("pixel_size").toDouble();

    for (QGraphicsItem* item : _scene->items())
    {
        if (is<CameraViewItem>(item))
        {
            CameraViewItem* rect = dynamic_cast<CameraViewItem*>(item);
            rect->setRect(-w*ps/2, -h*ps/2, w*ps, h*ps);
            item->setPos(x, y);
        }
    }
}