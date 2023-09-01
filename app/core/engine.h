#pragma once

#include <QObject>
#include <QScopedPointer>

#include "video.h"
#include "video3.h"

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

    Q_INVOKABLE QString getImage();

    Q_INVOKABLE QStringList camerasInfo();


private:
    void createQmlEngine();



    QScopedPointer<QQmlApplicationEngine> _qmlEngine;
    QScopedPointer<Serial> _serial;
    QString _photoCommand;

    V4L2 _videoDriver;
    Video3* _videoDriver3;
    QImage _image;
    QStringList _info;
};
