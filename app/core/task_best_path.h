#pragma once

#include <QObject>
#include <QThread>
#include <QString>
#include <QPoint>

#include "task_base.h"

class TaskBestPathPrivate;

class TaskBestPath : public QObject
{
    Q_OBJECT

public:
    TaskBestPath(QObject* parent = nullptr);
    ~TaskBestPath();

    Q_INVOKABLE void run(QPointF pos);

    Q_INVOKABLE void stopProgram();

signals:
    void message(QString);
    void finished();

private:
    TaskBestPathPrivate* const _impl;
    QScopedPointer<QThread> _thread;
};


class TaskBestPathPrivate : public TaskBase
{
    Q_OBJECT

public:
    TaskBestPathPrivate();

public slots:
    void run(QPointF startPoint);

private:

    friend class TaskBestPath;
};
