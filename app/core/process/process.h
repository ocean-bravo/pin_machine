#pragma once

#include <QProcess>

class Process : public QProcess
{
    Q_OBJECT

public:
    Process(QObject* parent = 0)
        : QProcess(parent)
    { }

    Q_INVOKABLE void start(const QString& program, const QStringList& arguments)
    {
        QProcess::start(program, arguments);
    }

    Q_INVOKABLE QString readAll()
    {
        return QString(QProcess::readAll());
    }
};
