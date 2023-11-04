#pragma once

#include <QObject>

class BlobItem;

class Task : public QObject
{
    Q_OBJECT

public:
    void moveTo(double x, double y);

    int updateBlobPosition(BlobItem* blob);

signals:
    void message(QString);
    void finished();
};
