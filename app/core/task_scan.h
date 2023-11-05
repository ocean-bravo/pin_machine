#pragma once

#include <QObject>
#include <QThread>
#include <QString>

#include "task_base.h"

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
    TaskScanPrivate* const _impl;
    QScopedPointer<QThread> _thread;
};


class TaskScanPrivate : public TaskBase
{
    Q_OBJECT

public:
    TaskScanPrivate();

public slots:
    void run(QString program);
    bool sendNextLine();
    void pauseProgram();

private:
    QStringList _codeLines;
    int _lineToSend = 0;

    double _xTarget;
    double _yTarget;

    friend class TaskScan;
};
