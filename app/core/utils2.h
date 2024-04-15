#pragma once

#include <QObject>
#include <QStringList>
#include <QMetaObject>
#include <QEventLoop>

#include <tuple>
#include "data_bus.h"

struct stopEx : std::exception {};

inline std::tuple<double, double, double> blobToDouble(const QString& blob)
{
    const QStringList coord = blob.split(" ", Qt::SkipEmptyParts);
    double x = coord[0].toDouble();
    double y = coord[1].toDouble();
    double dia = coord[2].toDouble();
    return {x, y, dia};
}

void waitPosXY(QPointF target, const QAtomicInteger<bool>& stop); // stopEx
void waitPosZ(double zTarget, const QAtomicInteger<bool>& stop); // stopEx

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

template<typename T>
void databusAction2(const QString& dbkey, T&& func)
{
    QObject::connect(&db(), &DataBus::valueChanged, [func = std::move(func), dbkey](const QString& key, const QVariant& value)
    {
        if (key == dbkey)
            func(value);
    });
}

// Не понимаю, что я тут делаю - как лямбда передается в функцию. Но работает.
template<typename T>
void databusAction(const QString& dbkey, T&& func)
{
    QObject::connect(&db(), &DataBus::valueChanged, [func = std::move(func), dbkey](const QString& key, const QVariant&)
    {
        if (key == dbkey)
            func();
    });
}
