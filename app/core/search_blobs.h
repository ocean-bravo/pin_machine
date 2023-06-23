#pragma once

#include <QObject>
#include <QThread>
#include <QString>
#include <QAtomicInteger>
#include <QMutex>


class Video4;

class SearchBlobsPrivate;

class SearchBlobs : public QObject
{
    Q_OBJECT

public:
    SearchBlobs(Video4* video, QObject* parent = nullptr);
    ~SearchBlobs();

    Q_INVOKABLE void run(QString program); // Не помню, кажется строка имеет ограничение 10000. Или QBYteArray

    bool sendNextLine();


    void startProgram();
    void pauseProgram();

    Q_INVOKABLE void stopProgram();
signals:

    void message(QString);
    void finished();

private:
    //void waitForGetPosition();
    void waitForSignal();
    void sleep(int);

    SearchBlobsPrivate* const _impl;
    QScopedPointer<QThread> _thread;
};



class SearchBlobsPrivate : public QObject
{
    Q_OBJECT

public:
    SearchBlobsPrivate(Video4* video);


public slots:
     void run(QString program);

    bool sendNextLine();



    void pauseProgram();

signals:

    void message(QString);
    void interrupt();
    void finished();

private:
    //void waitForGetPosition(double xTarget, double yTarget);
    void sleep(int);

    Video4* _video = nullptr;

    QStringList _codeLines;
    int _lineToSend = 0;

    double _xTarget;
    double _yTarget;
    void wait(int timeout) const;

    QMutex _mutex;
    QAtomicInteger<bool> _stop = false;
    friend class SearchBlobs;
};
