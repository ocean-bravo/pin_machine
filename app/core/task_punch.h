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
    Q_PROPERTY(bool isRunning  READ isRunning                      NOTIFY isRunningChanged  FINAL)
    Q_PROPERTY(bool isPaused   READ isPaused   WRITE setIsPaused   NOTIFY isPausedChanged   FINAL)
    Q_PROPERTY(bool stepByStep READ stepByStep WRITE setStepByStep NOTIFY stepByStepChanged FINAL)
    Q_PROPERTY(bool noPunch    READ noPunch    WRITE setNoPunch    NOTIFY noPunchChanged    FINAL)

public:
    TaskPunch(QObject* parent = nullptr);
    ~TaskPunch();

    Q_INVOKABLE void run(QString punchProgram, int width, int height, QString fourcc);
    Q_INVOKABLE void stopProgram();

signals:
    void message(QString);
    void finished();
    void isRunningChanged();
    void isPausedChanged();
    void stepByStepChanged();
    void noPunchChanged();

private:
    bool isRunning() const;

    bool stepByStep() const;
    void setStepByStep(bool state);

    bool noPunch() const;
    void setNoPunch(bool state);

    bool isPaused() const;
    void setIsPaused(bool state);

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

signals:
    void isRunningChanged();
    void isPausedChanged();

private:
    void waitForNextStep();

    QAtomicInteger<bool> _isPaused = false;
    QAtomicInteger<bool> _stepByStep = false;
    QAtomicInteger<bool> _noPunch = false;

    friend class TaskPunch;
};
