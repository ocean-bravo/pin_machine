#include "logger.h"
#include "utils.h"

#include <QFile>
#include <QSettings>
#include <QApplication>
#include <QDateTime>
#include <QtGlobal>
#include <QProcess>
#include <QTextStream>
#include <QtGlobal>
#include <QReadLocker>
#include <QMutexLocker>
#include <QWriteLocker>
#include <QHostAddress>

#include "udp_appender.h"

namespace {

const QString defaultUserLogPath = "logs/user.log";
const QString defaultCommonLogPath = "logs/common.log";

void loggerMessageHandler(QtMsgType, const QMessageLogContext&, const QString& msg)
{
    const Qt::HANDLE currentTheadId = QThread::currentThreadId();
    Logger::instance().common(msg, QDateTime::currentMSecsSinceEpoch(), currentTheadId);
}

}

LoggerPrivate::LoggerPrivate()
{

}

bool LoggerPrivate::logToFile() const
{
    return _logToFile;
}

void LoggerPrivate::setLogToFile(bool /*value*/)
{
    //USER(11, QString("parameter %1 changed to: %2").arg("logger/log/log_to_file").arg(value));
    //_logToFile = value;
    //_settings->setValue("log/log_to_file", value);

    //_settings->sync();

//    sync();
//    emit logToFileChanged();
}

void LoggerPrivate::init()
{
    //const QString settingsFilePath = appDir() + "logger.ini";

    //_settings = new QSettings(settingsFilePath, QSettings::IniFormat, this);

    _logToRemoteHost = true; //_settings->value("log/log_to_remote_host", false).toBool();

    if (_logToRemoteHost)
    {
        const QHostAddress logHost = QHostAddress("127.0.0.1");
        const quint16 logPort = 10000; //_settings->value("log/log_port", 10000).toUInt();

        _udpAppender.reset(new UdpAppender(logHost, logPort));
    }

    //_logToFile = _settings->value("log/log_to_file", false).toBool();

    // Аппендеры для записи в файл создаются все равно, как бы ни был установлен параметр, в файл или нет.
    // Чтобы когда из GUI включат сохранять в файл, было куда писать.
    // logrotate будет работать все равно. Место лога своей работы установлено в /dev/shm через параметр при запуске logrotate
//    const QString userLogPath = appDir() + _settings->value("log/user_path", defaultUserLogPath).toString();
//    const QString commonLogPath = appDir() + _settings->value("log/common_path", defaultCommonLogPath).toString();

//    _userFileAppender.reset(new FileAppender(userLogPath));
//    _commonFileAppender.reset(new FileAppender(commonLogPath));

//    if (QSysInfo::buildCpuArchitecture() != "arm")
//        return;

    qInstallMessageHandler(loggerMessageHandler);
    emit inited();
}

void LoggerPrivate::user(const QString& message)
{
    if (_logToRemoteHost)
        _udpAppender->append(message);

    if (_logToFile)
    {
        _userFileAppender->append(message);
        _commonFileAppender->append(message);
    }
}

void LoggerPrivate::common(const QString& message, qint64 time, Qt::HANDLE threadId)
{
    // Надоевшее сообщение от модуля Камеры: Unable to query the parameter info: "Invalid argument"
    if (message.contains("Unable to query"))
        return;

    if (message == "QObject::startTimer: Timers cannot be started from another thread")
        qt_noop();

    if (_logToRemoteHost)
        _udpAppender->append(QDateTime::fromMSecsSinceEpoch(time).toString("HH:mm:ss.zzz '[%1]' ").arg(threadIdToAlias(threadId)) + message);

    if (_logToFile)
        _commonFileAppender->append(message);
}

int LoggerPrivate::threadIdToAlias(Qt::HANDLE threadId)
{
    static const int invalidId = -1;
    int alias = _threadAliases.value(threadId, invalidId);

    if (alias == invalidId)
    {
        alias = _threadAliases.size();
        _threadAliases.insert(threadId, alias);
    }

    return alias;
}

bool Logger::logToFile() const
{
    return _impl->logToFile();
}

void Logger::setLogToFile(bool value)
{
    QMetaObject::invokeMethod(_impl, "setLogToFile", Qt::QueuedConnection, Q_ARG(bool, value));
}

Logger::Logger()
    : _impl(new LoggerPrivate)
    , _thread(new QThread)
{
    connect(this, &Logger::user,   _impl, &LoggerPrivate::user, Qt::QueuedConnection);
    connect(this, &Logger::common, _impl, &LoggerPrivate::common, Qt::QueuedConnection);

    connect(_impl, &LoggerPrivate::logToFileChanged, this, &Logger::logToFileChanged, Qt::QueuedConnection);
    connect(_impl, &LoggerPrivate::inited,           this, &Logger::inited, Qt::QueuedConnection);

    connect(_thread.data(), &QThread::finished, _impl, &QObject::deleteLater);
    connect(_thread.data(), &QThread::started,  _impl, &LoggerPrivate::init, Qt::QueuedConnection);

    _impl->moveToThread(_thread.data());
    _thread->start();
}

Logger::~Logger()
{
    _thread->quit();
    _thread->wait(1000);
}
