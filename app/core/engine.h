#pragma once

#include <QObject>
#include <QScopedPointer>

#include "video.h"
#include "video3.h"
#include "video4.h"

#include "my_image_provider.h"

class QQmlApplicationEngine;
class Serial;
class OpenCv;

class Engine : public QObject
{
    Q_OBJECT
    //Q_PROPERTY(type name READ name WRITE setName NOTIFY nameChanged)

public:
    explicit Engine(QObject* parent = nullptr);
    ~Engine();

    Q_INVOKABLE QStringList camerasInfo();

    Q_INVOKABLE QStringList removeDuplicatedBlobs(QStringList blobs);

signals:
    void imageChanged(QString id);

private:
    void createQmlEngine();


    QScopedPointer<QQmlApplicationEngine> _qmlEngine;
    QScopedPointer<Serial> _serial;

    V4L2 _videoDriver;
    Video3* _videoDriver3;
    Video4* _videoDriver4;
    QImage _image;

    QByteArray _imgPpm;
    QStringList _info;

    OpenCv* _openCv;
};
