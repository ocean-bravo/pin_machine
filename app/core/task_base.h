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
    void moveTo(double x, double y);
    void moveToAndWaitPosition(double x, double y);

    int updateBlobPosition(BlobItem* blob);

    // Не знаю куда приткнуть этот метод
    static void algorithmMatchPoints(QPointF firstRef, QPointF firstReal, BlobItem *secondRefBlob, BlobItem *secondRealBlob);

    double extractFromGcodeX(QString line, double defaultValue = qQNaN()) const;
    double extractFromGcodeY(QString line, double defaultValue = qQNaN()) const;
    double extractFromGcodeZ(QString line, double defaultValue = qQNaN()) const;
    int cameraId() const;


    QList<QGraphicsItem*> findShortestPath(QList<QGraphicsItem *> items, QPointF startPoint);

    QPointF currPos() const;

    void waitForNext();

protected:
    QMutex _mutex;
    QAtomicInteger<bool> _stop = false;

signals:
    void message(QString);
    void finished();
};
