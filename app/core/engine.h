#pragma once

#include <QObject>
#include <QScopedPointer>

#include "video.h"

class QQmlApplicationEngine;
class Serial;

class Engine : public QObject
{
    Q_OBJECT
    //Q_PROPERTY(type name READ name WRITE setName NOTIFY nameChanged)

public:
    explicit Engine(QObject* parent = nullptr);
    ~Engine();

    Q_INVOKABLE void update();
    Q_INVOKABLE void setPhotoCommand(QString);

private:
    void createQmlEngine();



    QScopedPointer<QQmlApplicationEngine> _qmlEngine;
    QScopedPointer<Serial> _serial;
    QString _photoCommand;

    V4L2 _videoDriver;
};
