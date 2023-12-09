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

    Q_INVOKABLE void run(int width, int height, QString fourcc); // Не помню, кажется строка имеет ограничение 10000. Или QBYteArray

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
    void run(int width, int height, QString fourcc);

private:

    friend class TaskCheckPixelSize;
};
