#pragma once

#include <QObject>
#include <QThread>
#include <QString>
#include <QAtomicInteger>

#include <QMutex>

class Video4;

class TestProgramPrivate;

class TaskScan;
class UpdateBlobs;

class TestProgram : public QObject
{
    Q_OBJECT

public:
    TestProgram(TaskScan* sb, UpdateBlobs* ub, QObject* parent = nullptr);
    ~TestProgram();

    Q_INVOKABLE void run(QString program);

    void startProgram();
    void pauseProgram();

    Q_INVOKABLE void stopProgram();

signals:
    void message(QString);
    void finished();

private:
    TestProgramPrivate* const _impl;
    QScopedPointer<QThread> _thread;
};



class TestProgramPrivate : public QObject
{
    Q_OBJECT

public:
    TestProgramPrivate(TaskScan* sb, UpdateBlobs* ub);


public slots:
    void run(QString program);


    void pauseProgram();

signals:

    void message(QString);
    void interrupt();
    void finished();

private:
    //void waitForGetPosition(double xTarget, double yTarget);

    void wait(int timeout) const;

    TaskScan* _sb = nullptr;
    UpdateBlobs* _ub = nullptr;

    QMutex _mutex;
    QAtomicInteger<bool> _stop = false;

    friend class TestProgram;

};
