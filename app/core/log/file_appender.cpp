#include "file_appender.h"
#include "diff_in_versions.h"

#include <QString>
#include <QDateTime>
#include <QTextStream>

FileAppender::FileAppender(const QString& path)
    : _file(new QFile(path))
{
    if (_file->open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
    {
        // TODO: сделать что-то если не удалось открыть файл
    }
}

void FileAppender::append(const QString &msg)
{
    QTextStream out(_file.data());
    out << QDateTime::currentDateTime().toString("dd-MM-yyyy HH:mm:ss.zzz; ");
    out << msg << endline;
    out.flush();    // Clear the buffered data
}
