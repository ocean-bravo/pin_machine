#include "utils2.h"

#include "data_bus.h"

#include <QTimer>
#include <QEventLoop>
#include <QScopeGuard>

namespace {

bool okValue(double targetValue, double currentValue)
{
    return std::abs(targetValue - currentValue) <= 0.003;
}

}

void waitPosXY(double xTarget, double yTarget)
{
    auto condition = [xTarget, yTarget]() -> bool
    {
        const QString status = db().value("status").toString();
        const double xPos = db().value("xPos").toDouble();
        const double yPos = db().value("yPos").toDouble();

        return (status == "Idle") && okValue(xTarget, xPos) && okValue(yTarget, yPos);
    };

    QEventLoop loop;
    QTimer::singleShot(10000, &loop, &QEventLoop::quit);

    QMetaObject::Connection conn = QObject::connect(&db(), &DataBus::valueChanged, &loop, [&condition, &loop](const QString& key, const QVariant&)
    {
        if ( key == "status" || key == "xPos" || key == "yPos")
        {
            if (condition())
                loop.quit();
        }
    });

    auto guard = qScopeGuard([=]()
    {
        QObject::disconnect(conn);
    });

    //у машины есть зона нечувствительности, т.е. она не реагирует на микроперемещения, около 0,002 мм
    if (condition())
        return;

    loop.exec();
}

void waitPosZ(double zTarget)
{
    auto condition = [zTarget]() -> bool
    {
        const QString status = db().value("status").toString();
        const double zPos = db().value("zPos").toDouble();

        return (status == "Idle") && okValue(zTarget, zPos);
    };

    QEventLoop loop;
    QTimer::singleShot(10000, &loop, &QEventLoop::quit);

    QMetaObject::Connection conn = QObject::connect(&db(), &DataBus::valueChanged, &loop, [&condition, &loop](const QString& key, const QVariant&)
    {
        if ( key == "status" || key == "zPos")
        {
            if (condition())
                loop.quit();
        }
    });

    auto guard = qScopeGuard([=]()
    {
        QObject::disconnect(conn);
    });

    //у машины есть зона нечувствительности, т.е. она не реагирует на микроперемещения, около 0,002 мм
    if (condition())
        return;

    loop.exec();
}


void waitDataBus(const QString& key, const QString& value)
{
    QEventLoop loop;
    QMetaObject::Connection conn = QObject::connect(&db(), &DataBus::valueChanged, &loop, [&loop, key, value](const QString& k, const QVariant& v)
    {
        if (k == key && v.toString() == value)
            loop.quit();
    });

    auto guard = qScopeGuard([=]()
    {
        QObject::disconnect(conn);
    });

    loop.exec();
}
