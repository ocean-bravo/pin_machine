#pragma once

#include <QObject>
#include <QThread>
#include <QString>
#include <QAtomicInteger>

class Video4;

class UpdateBlobsPrivate;

class UpdateBlobs : public QObject
{
    Q_OBJECT

public:
    UpdateBlobs(Video4* video, QObject* parent = nullptr);
    ~UpdateBlobs();

    Q_INVOKABLE void run();

    void startProgram();
    void pauseProgram();

    Q_INVOKABLE void stopProgram();

signals:
    void message(QString);

private:
    UpdateBlobsPrivate* const _impl;
    QScopedPointer<QThread> _thread;
};



class UpdateBlobsPrivate : public QObject
{
    Q_OBJECT

public:
    UpdateBlobsPrivate(Video4* video);
    QAtomicInteger<bool> stopProgram = false;


public slots:
    void run();
    //void run2();

    void pauseProgram();

signals:

    void message(QString);
    void interrupt();

private:
    void waitForGetPosition(double xTarget, double yTarget);

    Video4* _video = nullptr;

    double _xTarget;
    double _yTarget;
    void wait(int timeout) const;

};
