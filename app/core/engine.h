#pragma once

#include <QObject>
#include <QScopedPointer>

#include "video.h"
#include "video3.h"

#include "my_image_provider/my_image_provider.h"

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

    Q_INVOKABLE void getImage();

    Q_INVOKABLE QStringList camerasInfo();


signals:
    void imageCaptured();

private:
    void createQmlEngine();



    QScopedPointer<QQmlApplicationEngine> _qmlEngine;
    QScopedPointer<Serial> _serial;
    QString _photoCommand;

    V4L2 _videoDriver;
    Video3* _videoDriver3;
    QImage _image;

    QByteArray _imgPpm;
    QStringList _info;
};
