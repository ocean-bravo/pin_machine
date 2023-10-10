#pragma once

#include <QObject>
#include <QThread>
#include <QString>
#include <QAtomicInteger>


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

private:
    void waitForGetPosition();
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
    QAtomicInteger<bool> stopProgram = false;


public slots:
     void run(QString program);

    bool sendNextLine();


    void startProgram();
    void pauseProgram();

signals:

    void message(QString);
    void interrupt();

private:
    void waitForGetPosition(double xTarget, double yTarget);
    void waitForSignal();
    void sleep(int);

    Video4* _video = nullptr;

    QStringList _codeLines;
    int _lineToSend = 0;

    double _xTarget;
    double _yTarget;
    void wait(int timeout) const;
    using PointerToMember = void (SearchBlobsPrivate::*)();
    void waitForSignal(const PointerToMember &signal, int timeout) const;
    void waitForSignal(const QObject *object, const QMetaMethod &signal, int timeout) const;

};
