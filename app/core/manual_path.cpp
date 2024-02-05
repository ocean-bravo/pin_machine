#include "manual_path.h"

#include "data_bus.h"

#include "blob_item.h"

#include "utils2.h"



ManualPath::ManualPath(QObject *parent)
    : QObject(parent)
{

    databusAction2("manual_path_reset", [this](const QVariant& value)
    {
        if (value.toBool() == true)
        {
            _points.clear();
            db().insert("manual_path_reset", false); // Перезарядка

            buildPath();
        }
    });

    databusAction2("new_point_path", [this](const QVariant& value)
    {
        BlobItem* blob = value.value<BlobItem*>();

        _points.append(blob);

        buildPath();
    });
}

void ManualPath::buildPath()
{
    QList<QPointF> path;

    path.append(QPointF(0,0));

    for (BlobItem* blob : _points)
        path.append(blob->scenePos());

    path.append(QPointF(0,0));

    db().insert("punch_path", QVariant::fromValue(path));
}
