#pragma once

#include <QTimer>
#include <QObject>
#include <QEventLoop>
#include <QMetaMethod>
#include "utils.h"

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

//// Если вышли по таймауту - результат false
template<typename PointerToMemberFunction>
inline bool waitForSignal(const QObject* object, PointerToMemberFunction signal, int timeout)
{
    bool exitOnSignal = true;
    QEventLoop loop;
    QTimer::singleShot(timeout, [&loop, &exitOnSignal]() { exitOnSignal = false; loop.quit(); });

    static const int index = loop.metaObject()->indexOfMethod(QMetaObject::normalizedSignature("quit()"));
    static const QMetaMethod quitMetaMethod = loop.metaObject()->method(index);

    QObject::connect(object, QMetaMethod::fromSignal(signal), &loop, quitMetaMethod);
    loop.exec();
    return exitOnSignal;
}
