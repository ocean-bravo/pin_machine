#pragma once

#include <QObject>
#include <QThread>
#include <QString>

#include "task_base.h"

class TaskCheckCameraPrivate;

class TaskCheckCamera : public QObject
{
    Q_OBJECT

public:
    TaskCheckCamera(QObject* parent = nullptr);
    ~TaskCheckCamera();

    Q_INVOKABLE void run();
    Q_INVOKABLE void stopProgram();

signals:
    void message(QString);
    void finished();

private:
    TaskCheckCameraPrivate* const _impl;
    QScopedPointer<QThread> _thread;
};


class TaskCheckCameraPrivate : public TaskBase
{
    Q_OBJECT

public:
    TaskCheckCameraPrivate();

public slots:
    void run();

private:
    friend class TaskCheckCamera;
};
