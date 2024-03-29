#include "task_base.h"
#include "serial.h"
#include "utils.h"
#include "utils2.h"
#include "data_bus.h"
#include "blob_item.h"
#include "wait.h"
#include "openCv.h"
#include "video4.h"
#include "scene.h"
#include "settings.h"

#include <QJsonValue>
#include <QJsonArray>
#include <QJsonObject>
#include <QString>
#include <QRegularExpression>

void TaskBase::moveTo(double x, double y)
{
    const QString line = QString("G1 G90 F5000 X%1 Y%2").arg(toReal3(x), toReal3(y));
    emit message(line);
    serial().write(line.toLatin1() + "\n");

}

void TaskBase::moveToAndWaitPosition(double x, double y)
{
    moveTo(x, y);
    waitPosXY(x, y);
}

int TaskBase::updateBlobPosition(BlobItem *blob)
{
    double xTarget = blob->x();
    double yTarget = blob->y();
    double diaTarget = blob->rect().width();

    moveToAndWaitPosition(xTarget, yTarget);

    emit message("capturing ...");

    video().captureSmallRegion(diaTarget + 2);

    waitForSignal(&video(), &Video4::capturedSmallRegion, 2000);

    emit message("captured");

    QImage smallRegion = video().smallRegion();
    opencv().blobDetectorUpdated(smallRegion);

    waitForSignal(&opencv(), &OpenCv::smallRegionBlobChanged, 500);

    auto [ok, x, y, dia] = opencv().smallRegionBlob();

    if (ok && (dia / diaTarget > 2)) // неправильный блоб
        return 2;

    //qd() << "diameter " << dia;
    if (!ok)
    {
        emit message("blob NOT found");
        // Как то пометить блоб, что он не найден в этом месте
        return 1;
    }
    else
    {
        emit message("blob found");
        qd() << "blob found";

        // Обновили позицию и диаметро блоба
        scene().updateBlob(blob, x, y, dia);
        return 0;
    }
}

void TaskBase::algorithmMatchPoints(QPointF firstRef, QPointF firstReal, BlobItem* secondRefBlob, BlobItem* secondRealBlob)
{
    const QPointF secondRef = secondRefBlob->scenePos();
    const QPointF secondReal = secondRealBlob->scenePos();

    // 1.
    // Передвигаем плату в 1-ю реальную fid точку. 1-ые реальная и идеальная точки совпали.
    // Реальная точка стоит, к ней двигаем плату с идеальной точкой
    runOnThreadWait(&scene(), [=]() { scene().board()->moveBy(firstReal.x() - firstRef.x(), firstReal.y() - firstRef.y()); });

    //waitDataBus("step", "next"); db().insert("step", "");

    // 2.
    // Помещаем transform origin всей платы в первую идеальную fid точку.
    runOnThreadWait(&scene(), [=]() { scene().board()->setTransformOriginPoint(firstRef); });

    const double angleRef = QLineF(firstRef, secondRef).angle();
    const double angleReal = QLineF(firstReal, secondReal).angle();
    const double deltaAngle = angleRef - angleReal;

    // Довернули плату до реального угла
    runOnThreadWait(&scene(), [=]() { scene().board()->setRotation(deltaAngle); });

    //waitDataBus("step", "next"); db().insert("step", "");

    // 3. Перемещаю плату линии, соединяющей опорные точки, на половину разницы расстояний
    const double dx = secondRealBlob->scenePos().x() - secondRefBlob->scenePos().x();
    const double dy = secondRealBlob->scenePos().y() - secondRefBlob->scenePos().y();

    runOnThreadWait(&scene(), [=]() { scene().board()->moveBy(dx/2, dy/2); });

    //waitDataBus("step", "next"); db().insert("step", "");

    // 4. Восстанавливаю transform origin
    //runOnThreadWait(&scene(), []() { scene().board()->setTransformOriginPoint({0,0});});
}

// Возвращает defaultValue или число
double TaskBase::extractFromGcodeX(QString line, double defaultValue) const
{
    static QRegularExpression re(R"(.*X(-{0,1}\d{0,3}\.{0,1}\d{0,3}).*)");
    const QString value = re.match(line).captured(1);
    return value.isNull() ? defaultValue : value.toDouble();
}

// Возвращает defaultValue или число
double TaskBase::extractFromGcodeY(QString line, double defaultValue) const
{
    static QRegularExpression re(R"(.*Y(-{0,1}\d{0,3}\.{0,1}\d{0,3}).*)");
    const QString value = re.match(line).captured(1);
    return value.isNull() ? defaultValue : value.toDouble();
}

// Возвращает defaultValue или число
double TaskBase::extractFromGcodeZ(QString line, double defaultValue) const
{
    static QRegularExpression re(R"(.*Z(-{0,1}\d{0,3}\.{0,1}\d{0,3}).*)");
    const QString value = re.match(line).captured(1);
    return value.isNull() ? defaultValue : value.toDouble();
}

int TaskBase::cameraId() const
{
    const QString cameraName = settings().value("camera").toString();
    const QJsonArray cameras = db().value("cameras").toJsonArray();

    for (const QJsonValue& cameraInfo : cameras)
    {
        if (cameraInfo.toObject().value("name").toString().contains(cameraName))
            return cameraInfo.toObject().value("id").toInt();
    }
    qd() << "Camera: not found id for device name: " << cameraName;
    return -1;
}
