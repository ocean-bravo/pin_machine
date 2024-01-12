#pragma once

#include <QObject>
#include <QThread>
#include <QString>

#include "task_base.h"

class TaskFindPixelSizePrivate;

class TaskFindPixelSize : public QObject
{
    Q_OBJECT

public:
    TaskFindPixelSize(QObject* parent = nullptr);
    ~TaskFindPixelSize();

    Q_INVOKABLE void run();

    Q_INVOKABLE void stopProgram();

signals:
    void message(QString);
    void finished();

private:
    TaskFindPixelSizePrivate* const _impl;
    QScopedPointer<QThread> _thread;
};


class TaskFindPixelSizePrivate : public TaskBase
{
    Q_OBJECT

public:
    TaskFindPixelSizePrivate();

public slots:
    void run();

private:

    friend class TaskFindPixelSize;
};
