#pragma once

#include <QFuture>
#include <QRunnable>
#include <QThreadPool>

class TaskControl
{
public:
    TaskControl(QFutureInterfaceBase *f) : _f(f) {  }
    bool shouldRun() const { return !_f->isCanceled(); }
private:
    QFutureInterfaceBase *_f;
};

template <class T>
class ControllableTask
{
public:
    virtual ~ControllableTask() {}
    virtual T run(TaskControl& control) = 0;
};

template <typename T>
class RunControllableTask : public QFutureInterface<T> , public QRunnable
{
public:
    RunControllableTask(ControllableTask<T>* tsk) : _task(tsk) { }
    virtual ~RunControllableTask() { delete _task; }

    QFuture<T> start()
    {
        this->setRunnable(this);
        this->reportStarted();
        QFuture<T> future = this->future();
        QThreadPool::globalInstance()->start(this, /*m_priority*/ 0);
        return future;
    }

    void run()
    {
        if (this->isCanceled()) {
            this->reportFinished();
            return;
        }
        TaskControl control(this);
        _result = _task->run(control);
        if (!this->isCanceled()) {
            this->reportResult(_result);
        }
        this->reportFinished();
    }

    T  _result;
    ControllableTask<T>* _task;
};

class TaskExecutor {
public:
    template <class T>
    static QFuture<T> run(ControllableTask<T>* task)
    {
        return (new RunControllableTask<T>(task))->start();
    }

};

class Video4;

class SomeControllableTask : public ControllableTask<void>
{
public:
    SomeControllableTask(Video4* video);

    void run(TaskControl& control);

    bool sendNextLine();


    void startProgram();
    void pauseProgram();

    void stopProgram();
signals:

    void message(QString);

private:
    void waitForGetPosition();
    void waitForSignal();
    void sleep(int);

    Video4* _video = nullptr;

    QStringList codeLines;
    int lineToSend = 0;

    double xTarget;
    double yTarget;
    void wait(int timeout) const;
    using PointerToMember = void (SomeControllableTask::*)();
    void waitForSignal(const PointerToMember &signal, int timeout) const;
    void waitForSignal(const QObject *object, const QMetaMethod &signal, int timeout) const;
};
