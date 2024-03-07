#pragma once

#include <QObject>
#include <QThread>
#include <QString>
#include <QVariantMap>

#include "task_base.h"

class TaskTestScanUpdateCyclePrivate;

class TaskScan;
class TaskUpdate;

class TaskTestScanUpdateCycle : public QObject
{
    Q_OBJECT

public:
    TaskTestScanUpdateCycle(TaskScan* sb, TaskUpdate* ub, QObject* parent = nullptr);
    ~TaskTestScanUpdateCycle();

    Q_INVOKABLE void run(QString program);

    void startProgram();
    void pauseProgram();

    Q_INVOKABLE void stopProgram();

signals:
    void message(QString);
    void finished();

private:
    TaskTestScanUpdateCyclePrivate* const _impl;
    QScopedPointer<QThread> _thread;
};


class TaskTestScanUpdateCyclePrivate : public TaskBase
{
    Q_OBJECT

public:
    TaskTestScanUpdateCyclePrivate(TaskScan* sb, TaskUpdate* ub);

public slots:
    void run(QString program, QVariantMap options);

private:
    TaskScan* _sb = nullptr;
    TaskUpdate* _ub = nullptr;

    friend class TaskTestScanUpdateCycle;
};
