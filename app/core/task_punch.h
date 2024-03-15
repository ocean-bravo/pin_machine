#pragma once

#include <QObject>
#include <QThread>
#include <QString>
#include <QVariantMap>

#include "task_base.h"

class TaskPunchPrivate;

class TaskPunch : public QObject
{
    Q_OBJECT

public:
    TaskPunch(QObject* parent = nullptr);
    ~TaskPunch();

    Q_INVOKABLE void run(QString punchProgram, int width, int height, QString fourcc);
    Q_INVOKABLE void stopProgram();

signals:
    void message(QString);
    void finished();

private:
    TaskPunchPrivate* const _impl;
    QScopedPointer<QThread> _thread;
};


class TaskPunchPrivate : public TaskBase
{
    Q_OBJECT

public:
    TaskPunchPrivate();

public slots:
    void run(QString punchProgram, int width, int height, QString fourcc);

private:
    void waitForNextStep();

    friend class TaskPunch;
};
