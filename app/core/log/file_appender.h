#pragma once

#include "appender.h"

#include <QFile>
#include <QScopedPointer>

class QString;

class FileAppender : public Appender
{
public:
    FileAppender(const QString& path);
    void append(const QString& msg);

private:
    QScopedPointer<QFile> _file;
};
