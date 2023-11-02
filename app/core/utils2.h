#pragma once

#include <QObject>
#include <QStringList>
#include <QMetaObject>
#include <QEventLoop>

#include <tuple>

inline std::tuple<double, double, double> blobToDouble(const QString& blob)
{
    const QStringList coord = blob.split(" ", Qt::SkipEmptyParts);
    double x = coord[0].toDouble();
    double y = coord[1].toDouble();
    double dia = coord[2].toDouble();
    return {x, y, dia};
}

void waitPosXY(double xTarget, double yTarget);
void waitPosZ(double zTarget);

void waitDataBus(const QString& key, const QString& value);

template<typename Function>
auto runOnThread(QObject* targetObject, Function function)
{
    QMetaObject::invokeMethod(targetObject, std::move(function), Qt::QueuedConnection);
}

template<typename Function>
auto runOnThreadWait(QObject* targetObject, Function foo)
{
    QEventLoop loop;
    runOnThread(targetObject, [foo, &loop]()
    {
        foo();
        loop.quit();
    });

    loop.exec();
}

