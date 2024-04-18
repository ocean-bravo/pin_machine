#pragma once

#include <QTimer>
#include <QObject>
#include <QEventLoop>
#include <QMetaMethod>
#include "utils.h"

#include <functional>

/// Функция ожидания с обработкой событий потока, в котором она выполняется. Замена QThread::msleep.
inline void wait(int interval)
{
    if (interval > 0)
    {
        QEventLoop loop;
        QTimer::singleShot(interval, &loop, &QEventLoop::quit);
        loop.exec();
    }
}

// ХОрошо бы получать возвращаемое значение из сигнала
//// Если вышли по таймауту - результат false
template<typename PointerToMemberFunction>
inline bool waitForSignal(const QObject* object, PointerToMemberFunction signal, int timeout)
{
    bool exitOnSignal = true;
    QEventLoop loop;
    QTimer::singleShot(timeout, &loop, [&loop, &exitOnSignal]() { exitOnSignal = false; loop.quit(); });

    static const int index = loop.metaObject()->indexOfMethod(QMetaObject::normalizedSignature("quit()"));
    static const QMetaMethod quitMetaMethod = loop.metaObject()->method(index);

    QObject::connect(object, QMetaMethod::fromSignal(signal), &loop, quitMetaMethod);
    loop.exec();
    return exitOnSignal;
}

// 0 - выход по таймауту
// 1 - выход по 1 сигналу
// 2 - выход по 2 сигналу

// connect state to a functor or function pointer (without context)
template <typename Func1>
inline typename QtPrivate::QEnableIf<!QtPrivate::FunctionPointer<Func1>::IsPointerToMemberFunction &&
        !std::is_same<const char*, Func1>::value, QMetaObject::Connection>::Type
connectToEvent(const QString &scxmlEventSpec, Func1 slot, Qt::ConnectionType type = Qt::AutoConnection)
{}

template<typename PointerToMemberFunction1> //, typename PointerToMemberFunction2
inline int waitForSignals(int timeout, const QObject* object1, PointerToMemberFunction1 signal1) //, const QObject* object2,  PointerToMemberFunction2 signal2)
{
    int exit = 0;
    QEventLoop loop;
    QTimer::singleShot(timeout, &loop, [&loop]() { loop.quit(); });

    //std::function<void()> callback = [&loop, &exit]() { exit = 1; loop.quit(); };

    auto a = (const QObject*)object1;



    //QObject::connect(object1, QMetaMethod::fromSignal(signal1), context, callback, Qt::AutoConnection);
    // /QObject::connect(object2, QMetaMethod::fromSignal(signal2), [&loop, &exit]() { exit = 2; loop.quit(); });
    QMetaObject::Connection conn1 =  QObject::connect(object1, signal1, a, [&loop, &exit]() { exit = 1; loop.quit(); });
    // //QObject::connect(object2, QMetaMethod::fromSignal(signal2), [&loop, &exit]() { exit = 2; loop.quit(); });

    auto guard = qScopeGuard([=]()
    {
        QObject::disconnect(conn1);
        //disconnect(conn2);
    });


    loop.exec();

    return exit;
}
