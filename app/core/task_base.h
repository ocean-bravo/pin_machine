#pragma once

#include <QMutex>
#include <QObject>
#include <QAtomicInteger>

class BlobItem;
class QGraphicsItem;


class TaskBase : public QObject
{
    Q_OBJECT

public:
    void moveTo(QPointF pos);
    void moveToAndWaitPosition(QPointF pos); // stopEx

    int updateBlobPosition(BlobItem* blob, QVariantMap options);

    // Не знаю куда приткнуть этот метод
    static void algorithmMatchPoints(QPointF firstRef, QPointF firstReal, BlobItem *secondRefBlob, BlobItem *secondRealBlob);

    double extractFromGcodeX(QString line, double defaultValue = qQNaN()) const;
    double extractFromGcodeY(QString line, double defaultValue = qQNaN()) const;
    double extractFromGcodeZ(QString line, double defaultValue = qQNaN()) const;
    int cameraId() const;


    // Список уникальных ширин форматов, т.е., например, [160, 320, 640, 1024]
    QList<int> uniqueWidths() const;

    // Любая из доступных высота для данной ширины изображения
    int anyHeightForWidth(int width) const;

    // Любой из доступных форматов для данной ширины и высоты изображения.
    QString anyFourcc(int width, int height) const;


    QList<BlobItem*> findShortestPath(QList<BlobItem*> items, QPointF startPoint);

    QPointF currPos() const;

    void waitForNext();

protected:
    static QMutex _someTaskInProgress;
    QAtomicInteger<bool> _stop = false;
    QAtomicInteger<bool> _running = false;

signals:
    void message(QString);
    void finished();
};
