#pragma once

#include <QObject>
#include <QScopedPointer>

//#include "video.h"
//#include "video3.h"
#include "video4.h"


class QQmlApplicationEngine;
class Serial;

class Engine : public QObject
{
    Q_OBJECT
    //Q_PROPERTY(type name READ name WRITE setName NOTIFY nameChanged)

public:
    explicit Engine(QObject* parent = nullptr);
    ~Engine();

    //Q_INVOKABLE QStringList camerasInfo();

    Q_INVOKABLE void save(const QString &url);
    Q_INVOKABLE void load(const QString &url);
    Q_INVOKABLE void capture();

signals:
    void imageChanged(QString id);

private:
    void createQmlEngine();

    QScopedPointer<QQmlApplicationEngine> _qmlEngine;

    //V4L2 _videoDriver;
//    Video3* _videoDriver3;
//    Video4* _videoDriver4;
    //QImage _image;

    //QByteArray _imgPpm;
    //QStringList _info;
};
