#pragma once

#include <QObject>
#include <QThread>
#include <QString>
#include <QPointF>

#include "task_base.h"

// Досканировать позицию по клику мыши пользователем

class TaskScanPositionPrivate;

class TaskScanPosition : public QObject
{
    Q_OBJECT

public:
    TaskScanPosition(QObject* parent = nullptr);
    ~TaskScanPosition();

    void run(QPointF pos, QString sceneFile);

    void stopProgram();

signals:
    void message(QString);
    void finished();

private:
    TaskScanPositionPrivate* const _impl;
    QScopedPointer<QThread> _thread;
};


class TaskScanPositionPrivate : public TaskBase
{
    Q_OBJECT

public:
    TaskScanPositionPrivate();

public slots:
    void run(QPointF pos, QString sceneFile);

private:

    friend class TaskScanPosition;
};
