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
    Punch(Video4* video, QObject* parent = nullptr);
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
    PunchPrivate(Video4* video);

public slots:
    void run();

signals:

private:
    Video4* _video = nullptr;

    QMutex _mutex;
    QAtomicInteger<bool> _stop = false;

    friend class Punch;
};
