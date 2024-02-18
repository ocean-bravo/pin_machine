#include "file_system_watcher.h"


FileSystemWatcher::FileSystemWatcher(QObject* parent)
    : QFileSystemWatcher(parent)
{

}

bool FileSystemWatcher::addPath(const QString &file)
{
    return QFileSystemWatcher::addPath(file);
}

QStringList FileSystemWatcher::addPaths(const QStringList &files)
{
    return QFileSystemWatcher::addPaths(files);
}

bool FileSystemWatcher::removePath(const QString &file)
{
    return QFileSystemWatcher::removePath(file);
}

QStringList FileSystemWatcher::removePaths(const QStringList &files)
{
    return QFileSystemWatcher::removePaths(files);
}

QStringList FileSystemWatcher::files() const
{
    return QFileSystemWatcher::files();
}

QStringList FileSystemWatcher::directories() const
{
    return QFileSystemWatcher::directories();
}
