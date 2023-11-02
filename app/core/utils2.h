#pragma once

#include <QObject>
#include <QStringList>
#include <QMetaObject>

#include <tuple>

inline std::tuple<double, double, double> blobToDouble(const QString& blob)
{
    const QStringList coord = blob.split(" ", Qt::SkipEmptyParts);
    double x = coord[0].toDouble();
    double y = coord[1].toDouble();
    double dia = coord[2].toDouble();
    return {x, y, dia};
}

void waitForGetPosition(double xTarget, double yTarget);

template<typename Function>
auto runOnThread(QObject* targetObject, Function function)
{
    QMetaObject::invokeMethod(targetObject, std::move(function), Qt::QueuedConnection);
}

template<typename PointerToMemberFunction>
inline void runLater(const QObject* object, PointerToMemberFunction member)
{
    runOnThread(object, [object, member]() { object->*member(); });
}
