#pragma once

#include <QObject>

class BlobItem;

class ManualPath : public QObject
{
    Q_OBJECT

public:
    ManualPath(QObject* parent = nullptr);



private:
    void buildPath();

    QList<BlobItem*> _points;
};
