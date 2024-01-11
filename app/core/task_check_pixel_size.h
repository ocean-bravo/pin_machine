#pragma once

#include <QObject>
#include <QThread>
#include <QString>

#include "task_base.h"

class TaskCheckPixelSizePrivate;

class TaskCheckPixelSize : public QObject
{
    Q_OBJECT

public:
    TaskCheckPixelSize(QObject* parent = nullptr);
    ~TaskCheckPixelSize();

    Q_INVOKABLE void run();

    Q_INVOKABLE void stopProgram();

signals:
    void message(QString);
    void finished();

private:
    TaskCheckPixelSizePrivate* const _impl;
    QScopedPointer<QThread> _thread;
};


class TaskCheckPixelSizePrivate : public TaskBase
{
    Q_OBJECT

public:
    TaskCheckPixelSizePrivate();

public slots:
    void run();

private:

    friend class TaskCheckPixelSize;
};
