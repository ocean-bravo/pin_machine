#pragma once

#include <QObject>
#include <QThread>
#include <QString>
#include <QAtomicInteger>
#include <QMutex>

#include "task.h"

class Video4;

class TaskScanPrivate;

class TaskScan : public QObject
{
    Q_OBJECT

public:
    TaskScan(QObject* parent = nullptr);
    ~TaskScan();

    Q_INVOKABLE void run(QString program); // Не помню, кажется строка имеет ограничение 10000. Или QBYteArray

    bool sendNextLine();

    Q_INVOKABLE void stopProgram();

signals:
    void message(QString);
    void finished();

private:
    //void waitForGetPosition();
    void waitForSignal();
    void sleep(int);

    TaskScanPrivate* const _impl;
    QScopedPointer<QThread> _thread;
};



class TaskScanPrivate : public Task
{
    Q_OBJECT

public:
    TaskScanPrivate();

public slots:
    void run(QString program);
    bool sendNextLine();
    void pauseProgram();

signals:
    void interrupt();

private:
    //void waitForGetPosition(double xTarget, double yTarget);
    void sleep(int);

    QStringList _codeLines;
    int _lineToSend = 0;

    double _xTarget;
    double _yTarget;
    void wait(int timeout) const;

    QMutex _mutex;
    QAtomicInteger<bool> _stop = false;
    friend class TaskScan;
};
