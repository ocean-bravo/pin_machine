#include "utils2.h"

#include "data_bus.h"

#include <QTimer>
#include <QEventLoop>
#include <QScopeGuard>

void waitForGetPosition(double xTarget, double yTarget)
{
    auto condition = [xTarget, yTarget]() -> bool
    {
        const QString status = db().value("status").toString();
        const double xPos = db().value("xPos").toDouble();
        const double yPos = db().value("yPos").toDouble();

        return (status == "Idle") && (std::abs(xTarget - xPos) <= 0.003) && (std::abs(yTarget - yPos) <= 0.003);
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
