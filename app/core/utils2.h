#pragma once

#include <QStringList>
#include <tuple>

inline std::tuple<double, double, double> blobToDouble(const QString& blob)
{
    const QStringList coord = blob.split(" ", Qt::SkipEmptyParts);
    double x = coord[0].toDouble();
    double y = coord[1].toDouble();
    double dia = coord[2].toDouble();
    return {x, y, dia};
}
