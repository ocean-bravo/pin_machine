#include "manual_path.h"

#include "data_bus.h"
#include "blob_item.h"
#include "utils2.h"

ManualPath::ManualPath(QObject *parent)
    : QObject(parent)
{
    databusAction2("punchpath_manual_reset", [this](const QVariant& value)
    {
        if (value.toBool() == true)
        {
            db("punchpath_manual_reset") = false; // Перезарядка

            _blobs.clear();
            createPathToDraw();
        }
    });

    databusAction2("punchpath_manual_add_point", [this](const QVariant& value)
    {
        BlobItem* blob = value.value<BlobItem*>();

        // Без повторов точек
        if (_blobs.contains(blob))
            return;

        // Нужно только punch точки
        if (!blob->isPunch())
            return;

        _blobs.push_back(blob);

        db("punchpath") = QVariant::fromValue(_blobs);
        createPathToDraw();
    });
}

void ManualPath::createPathToDraw()
{
    QList<QPointF> path;

    const QPointF startPoint = db().value("punchpath_start_point").toPointF();

    path.append(startPoint);

    for (BlobItem* blob : qAsConst(_blobs))
        path.append(blob->scenePos());

    path.append(startPoint);

    db("punchpath_draw") = QVariant::fromValue(path);
}
