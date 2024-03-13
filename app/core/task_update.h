#pragma once

#include <QObject>
#include <QThread>
#include <QString>
#include <QVariantMap>

#include "task_base.h"

class TaskUpdatePrivate;

class TaskUpdate : public QObject
{
    Q_OBJECT

public:
    TaskUpdate(QObject* parent = nullptr);
    ~TaskUpdate();

    Q_INVOKABLE void run(int width, int height, QString fourcc);
    Q_INVOKABLE void stopProgram();

signals:
    void message(QString);
    void finished();

private:
    TaskUpdatePrivate* const _impl;
    QScopedPointer<QThread> _thread;
};


class TaskUpdatePrivate : public TaskBase
{
    Q_OBJECT

public:
    TaskUpdatePrivate();

public slots:
    void run(int width, int height, QString fourcc);

private:
    friend class TaskUpdate;
};
