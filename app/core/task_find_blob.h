#pragma once

#include <QObject>
#include <QThread>
#include <QString>
#include <QPointF>

#include "task_base.h"

// Досканировать позицию по клику мыши пользователем

class TaskFindBlobPrivate;

class TaskFindBlob : public QObject
{
    Q_OBJECT

public:
    TaskFindBlob(QObject* parent = nullptr);
    ~TaskFindBlob();

    void run(QPointF pos);

    void stopProgram();

signals:
    void message(QString);
    void finished();

private:
    TaskFindBlobPrivate* const _impl;
    QScopedPointer<QThread> _thread;
};


class TaskFindBlobPrivate : public TaskBase
{
    Q_OBJECT

public:
    TaskFindBlobPrivate();

public slots:
    void run(QPointF pos);

private:

    friend class TaskFindBlob;
};
