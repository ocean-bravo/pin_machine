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

protected:
    QMutex _mutex;
    QAtomicInteger<bool> _stop = false;

signals:
    void message(QString);
    void finished();
};
