#pragma once

#include <QObject>
#include <QThread>
#include <QString>

#include "task_base.h"

class TaskTestAlgoPrivate;

class TaskTestAlgo : public QObject
{
    Q_OBJECT

public:
    TaskTestAlgo(QObject* parent = nullptr);
    ~TaskTestAlgo();

    Q_INVOKABLE void run();
    Q_INVOKABLE void stopProgram();

signals:
    void message(QString);
    void finished();

private:
    TaskTestAlgoPrivate* const _impl;
    QScopedPointer<QThread> _thread;
};


class TaskTestAlgoPrivate : public TaskBase
{
    Q_OBJECT

public:
    TaskTestAlgoPrivate();

public slots:
    void run();

private:
    friend class TaskTestAlgo;
};
