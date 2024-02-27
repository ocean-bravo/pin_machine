#pragma once

#include <QObject>
#include <QScopedPointer>

//#include "video.h"
//#include "video3.h"
#include "video4.h"

class Serial;

#include <QQmlApplicationEngine>

class EnhancedQmlApplicationEngine : public QQmlApplicationEngine
{
    Q_OBJECT

public:
    explicit EnhancedQmlApplicationEngine(QObject* parent = nullptr);

    Q_INVOKABLE void clearCache();
};

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
    //Q_INVOKABLE void capture();

    // Q_INVOKABLE void capture1();
    // Q_INVOKABLE void capture2();
    // Q_INVOKABLE void corr();

    Q_INVOKABLE void reload();

signals:
    void imageChanged(QString id);

private:
    void createQmlEngine();

    QScopedPointer<EnhancedQmlApplicationEngine> _qmlEngine;

    //V4L2 _videoDriver;
//    Video3* _videoDriver3;
//    Video4* _videoDriver4;
    //QImage _image;

    //QByteArray _imgPpm;
    //QStringList _info;
};
