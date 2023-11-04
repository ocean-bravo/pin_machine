#pragma once

#include <QObject>
#include <QThread>
#include <QString>
#include <QAtomicInteger>
#include <QMutex>

#include "task.h"

class Video4;

class PunchPrivate;

class Punch : public QObject
{
    Q_OBJECT

public:
    Punch(QObject* parent = nullptr);
    ~Punch();

    Q_INVOKABLE void run();
    Q_INVOKABLE void stopProgram();

signals:
    void message(QString);
    void finished();

private:
    PunchPrivate* const _impl;
    QScopedPointer<QThread> _thread;
};


class PunchPrivate : public Task
{
    Q_OBJECT

public:
    PunchPrivate();

public slots:
    void run();

signals:

private:
    QMutex _mutex;
    QAtomicInteger<bool> _stop = false;

    friend class Punch;
};
