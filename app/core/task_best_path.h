#pragma once

#include <QObject>
#include <QThread>
#include <QString>

#include "task_base.h"

class TaskBestPathPrivate;

class TaskBestPath : public QObject
{
    Q_OBJECT

public:
    TaskBestPath(QObject* parent = nullptr);
    ~TaskBestPath();

    Q_INVOKABLE void run();

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
    void run();

private:

    friend class TaskBestPath;
};
