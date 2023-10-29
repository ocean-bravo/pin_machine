#pragma once

#include <QObject>
#include <QThread>
#include <QString>
#include <QAtomicInteger>

class Video4;

class TestProgramPrivate;

class TestProgram : public QObject
{
    Q_OBJECT

public:
    TestProgram(QObject* parent = nullptr);
    ~TestProgram();

    Q_INVOKABLE void run();

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
    TestProgramPrivate();
    QAtomicInteger<bool> stopProgram = false;


public slots:
    void run();


    void pauseProgram();

signals:

    void message(QString);
    void interrupt();
    void finished();

private:
    //void waitForGetPosition(double xTarget, double yTarget);

    void wait(int timeout) const;

};
