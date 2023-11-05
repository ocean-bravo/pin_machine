#include "task_base.h"
#include "serial.h"
#include "utils.h"
#include "utils2.h"
#include "blob_item.h"
#include "wait.h"
#include "openCv.h"
#include "video4.h"
#include "scene.h"

#include <QString>

void TaskBase::moveTo(double x, double y)
{
    const QString line = QString("G1 G90 F5000 X%1 Y%2").arg(toReal3(x), toReal3(y));
    serial().write(line.toLatin1() + "\n");

}

int TaskBase::updateBlobPosition(BlobItem *blob)
{
    double xTarget = blob->x();
    double yTarget = blob->y();
    double diaTarget = blob->rect().width();

    moveTo(xTarget, yTarget);

    waitForGetPosition(xTarget, yTarget);

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
