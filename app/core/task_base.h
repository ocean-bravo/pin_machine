#pragma once

#include <QMutex>
#include <QObject>
#include <QAtomicInteger>

class BlobItem;

class TaskBase : public QObject
{
    Q_OBJECT

public:
    void moveTo(double x, double y);

    int updateBlobPosition(BlobItem* blob);

    // Не знаю куда приткнуть этот метод
    static void algorithmMatchPoints(QPointF firstRef, QPointF firstReal, QPointF secondRef, QPointF secondReal);

protected:
    QMutex _mutex;
    QAtomicInteger<bool> _stop = false;

signals:
    void message(QString);
    void finished();
};
