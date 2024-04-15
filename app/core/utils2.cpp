#include "utils2.h"

#include "data_bus.h"

#include <QTimer>
#include <QEventLoop>
#include <QScopeGuard>
#include <QPointF>

namespace {

bool okValue(double targetValue, double currentValue)
{
    return std::abs(targetValue - currentValue) <= 0.003;
}

}

void waitPosXY(QPointF target, const QAtomicInteger<bool>& stop) // stopEx
{
    auto condition = [target]() -> bool
    {
        const QString status = db().value("status").toString();
        const double xPos = db().value("xPos").toDouble();
        const double yPos = db().value("yPos").toDouble();

        return (status == "Idle") && okValue(target.x(), xPos) && okValue(target.y(), yPos);
    };

    QEventLoop loop;
    QTimer::singleShot(10000, &loop, &QEventLoop::quit);

    QMetaObject::Connection conn = QObject::connect(&db(), &DataBus::valueChanged, &loop, [&condition, &loop, &stop](const QString& key, const QVariant&)
    {
        if (stop)
            throw stopEx();

        if ( key == "status" || key == "xPos" || key == "yPos")
        {
            if (condition())
            {
                loop.quit();
                return;
            }
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

void waitPosZ(double zTarget, const QAtomicInteger<bool>& stop) // stopEx
{
    auto condition = [zTarget]() -> bool
    {
        const QString status = db().value("status").toString();
        const double zPos = db().value("zPos").toDouble();

        return (status == "Idle") && okValue(zTarget, zPos);
    };

    QEventLoop loop;
    QTimer::singleShot(10000, &loop, &QEventLoop::quit);

    QMetaObject::Connection conn = QObject::connect(&db(), &DataBus::valueChanged, &loop, [&condition, &loop, &stop](const QString& key, const QVariant&)
    {
        if (stop)
            throw stopEx();

        if ( key == "status" || key == "zPos")
        {
            if (condition())
            {
                loop.quit();
                return;
            }
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
