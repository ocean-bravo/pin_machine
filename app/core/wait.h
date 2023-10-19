#pragma once

#include <QTimer>
#include <QObject>
#include <QEventLoop>
#include <QMetaMethod>

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
inline bool waitForSignal(const QObject* object, const QMetaMethod& signal, int timeout)
{
    bool exitOnSignal = true;
    QEventLoop loop;
    QTimer::singleShot(timeout, &loop, [&loop, &exitOnSignal]() { exitOnSignal = false; loop.quit(); });

    const int index = loop.metaObject()->indexOfMethod(QMetaObject::normalizedSignature("quit()"));
    const QMetaMethod quitMetaMethod = loop.metaObject()->method(index);

    QObject::connect(object, signal, &loop, quitMetaMethod);
    loop.exec();
    return exitOnSignal;
}
