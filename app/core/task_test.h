#pragma once

#include <QObject>
#include <QThread>
#include <QString>

#include "task_base.h"

class TaskTestPrivate;

class TaskScan;
class TaskUpdate;

class TaskTest : public QObject
{
    Q_OBJECT

public:
    TaskTest(TaskScan* sb, TaskUpdate* ub, QObject* parent = nullptr);
    ~TaskTest();

    Q_INVOKABLE void run(QString program);

    void startProgram();
    void pauseProgram();

    Q_INVOKABLE void stopProgram();

signals:
    void message(QString);
    void finished();

private:
    TaskTestPrivate* const _impl;
    QScopedPointer<QThread> _thread;
};


class TaskTestPrivate : public TaskBase
{
    Q_OBJECT

public:
    TaskTestPrivate(TaskScan* sb, TaskUpdate* ub);

public slots:
    void run(QString program);

private:
    TaskScan* _sb = nullptr;
    TaskUpdate* _ub = nullptr;

    friend class TaskTest;
};
