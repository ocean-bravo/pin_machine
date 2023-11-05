#pragma once

#include <QObject>
#include <QThread>
#include <QString>
#include <QAtomicInteger>
#include <QMutex>
#include "task.h"

class Video4;

class TaskUpdatePrivate;

class TaskUpdate : public QObject
{
    Q_OBJECT

public:
    TaskUpdate(QObject* parent = nullptr);
    ~TaskUpdate();

    Q_INVOKABLE void run();
    Q_INVOKABLE void stopProgram();

signals:
    void message(QString);
    void finished();

private:
    TaskUpdatePrivate* const _impl;
    QScopedPointer<QThread> _thread;
};


class TaskUpdatePrivate : public Task
{
    Q_OBJECT

public:
    TaskUpdatePrivate();

public slots:
    void run();

signals:

private:
    QMutex _mutex;
    QAtomicInteger<bool> _stop = false;

    friend class TaskUpdate;
};
