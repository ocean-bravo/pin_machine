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
            db("manual_path_reset") = false; // Перезарядка

            createPathToDraw();
        }
    });

    databusAction2("manual_path_add_point", [this](const QVariant& value)
    {
        BlobItem* blob = value.value<BlobItem*>();

        // Без повторов точек
        if (_points.contains(blob))
            return;

        // Нужно только punch точки
        if (!blob->isPunch())
            return;

        _points.append(blob);

        createPathToDraw();
    });
}

void ManualPath::createPathToDraw()
{
    QList<QPointF> path;

    const QPointF startPoint = db().value("punchpath_start_point").toPointF();

    path.append(startPoint);

    for (BlobItem* blob : qAsConst(_points))
        path.append(blob->scenePos());

    path.append(startPoint);

    db("punchpath") = QVariant::fromValue(path);
}
