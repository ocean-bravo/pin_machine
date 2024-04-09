#pragma once

#include <QMap>
#include <QFile>
#include <QTimer>
#include <QObject>
#include <QThread>
#include <QReadWriteLock>
#include <QScopedPointer>

#include "appender.h"
#include "singleton.h"

class QTimer;
class QSettings;
class LoggerPrivate;

class Logger : public QObject, public Singleton<Logger>
{
    Q_OBJECT
    Q_PROPERTY(bool logToFile READ logToFile WRITE setLogToFile NOTIFY logToFileChanged) // Логгировать ли в файл

    Q_PROPERTY(QString log WRITE appendToLog)

public:
    // Пока предварительно - По какому пути пишутся общие логи. Могут писаться на sd карту, могут на RAM диск.
    QString commonLogPath() const;

    // Логгирование в файл. При устрановке значения перезагрузка не нужна. Изменения принимаются немедленно
    bool logToFile() const;
    void setLogToFile(bool value);

    void appendToLog(const QString& msg);

signals:
    Q_INVOKABLE void user(const QString& message);
    void common(const QString& message, qint64 time, Qt::HANDLE threadId, qint32 tid = 0);
    void logToFileChanged();
    void inited();
    void newMessage(QString message); // Для QML

private:
    Logger();
    ~Logger();

    LoggerPrivate* const _impl;
    QScopedPointer<QThread> _thread;
    friend class Singleton<Logger>;
};

class LoggerPrivate : public QObject
{
    Q_OBJECT

public:
    LoggerPrivate();

    bool logToFile() const;

public slots:
    void init();
    void user(const QString& message);
    void common(const QString& message, qint64 time, Qt::HANDLE threadId, qint32 tid = 0);
    void setLogToFile(bool value);

signals:
    void logToFileChanged(); // для информирования GUI
    void inited();
    void newMessage(QString message); // Для QML

private:
    int threadIdToAlias(Qt::HANDLE threadId);

    //QSettings* _settings = nullptr;
    bool _logToRemoteHost = true;
    QAtomicInteger<bool> _logToFile = false;
    QScopedPointer<Appender> _userFileAppender;
    QScopedPointer<Appender> _commonFileAppender;
    QScopedPointer<Appender> _udpAppender;
    QMap<Qt::HANDLE, int> _threadAliases;
};
