#pragma once

#include <QFileSystemWatcher>

class FileSystemWatcher : public QFileSystemWatcher
{
    Q_OBJECT

public:
    FileSystemWatcher(QObject* parent = nullptr);

    Q_INVOKABLE bool addPath(const QString &file);
    Q_INVOKABLE QStringList addPaths(const QStringList &files);
    Q_INVOKABLE bool removePath(const QString &file);
    Q_INVOKABLE QStringList removePaths(const QStringList &files);

    Q_INVOKABLE QStringList files() const;
    Q_INVOKABLE QStringList directories() const;
};
