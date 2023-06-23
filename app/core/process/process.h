#pragma once

#include <QProcess>

class Process : public QProcess
{
    Q_OBJECT
    Q_PROPERTY(QString standardError READ readAllStandardError NOTIFY standardErrorChanged)
    Q_PROPERTY(QString standardOutput READ readAllStandardOutput NOTIFY standardOutputChanged)

public:
    Process(QObject* parent = 0)
        : QProcess(parent)
    {
        connect(this, &QProcess::readyReadStandardError,  this, &Process::standardErrorChanged);
        connect(this, &QProcess::readyReadStandardOutput, this, &Process::standardOutputChanged);
    }

    Q_INVOKABLE void start(const QString& program, const QStringList& arguments)
    {
        QProcess::start(program, arguments);
    }

    Q_INVOKABLE QString readAll()
    {
        return QString(QProcess::readAll());
    }

signals:
    void standardErrorChanged();
    void standardOutputChanged();
};
