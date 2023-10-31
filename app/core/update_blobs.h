#pragma once

#include <QObject>
#include <QThread>
#include <QString>
#include <QAtomicInteger>
#include <QMutex>

class Video4;

class UpdateBlobsPrivate;

class UpdateBlobs : public QObject
{
    Q_OBJECT

public:
    UpdateBlobs(Video4* video, QObject* parent = nullptr);
    ~UpdateBlobs();

    Q_INVOKABLE void run();
    Q_INVOKABLE void stopProgram();

signals:
    void message(QString);
    void finished();

private:
    UpdateBlobsPrivate* const _impl;
    QScopedPointer<QThread> _thread;
};


class UpdateBlobsPrivate : public QObject
{
    Q_OBJECT

public:
    UpdateBlobsPrivate(Video4* video);

public slots:
    void run();

signals:
    void message(QString);
    void finished();

private:
    Video4* _video = nullptr;

    QMutex _mutex;
    QAtomicInteger<bool> _stop = false;

    friend class UpdateBlobs;
};
