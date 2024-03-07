#pragma once

#include <QObject>
#include <QThread>
#include <QString>
#include <QVariantMap>

#include "task_base.h"

class TaskCheckCameraPrivate;

class TaskCheckCamera : public QObject
{
    Q_OBJECT

public:
    TaskCheckCamera(QObject* parent = nullptr);
    ~TaskCheckCamera();

    Q_INVOKABLE void run(QVariantMap options);
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
    void run(QVariantMap options);

private:
    friend class TaskCheckCamera;
};
