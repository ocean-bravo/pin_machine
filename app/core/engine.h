#pragma once

#include <QObject>
#include <QScopedPointer>

//#include "video.h"
//#include "video3.h"
#include "video4.h"

class Serial;
class QQuickWidget;
class MainWindow;
class MainWindow3;
class WidgetAnchor;
class GraphicsView;

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


    void reinit();
    //Q_INVOKABLE void capture();

    // Q_INVOKABLE void capture1();
    // Q_INVOKABLE void capture2();
    // Q_INVOKABLE void corr();

    Q_INVOKABLE void reload();

    Q_INVOKABLE QStringList filesInDirectory(QString dir) const;
    Q_INVOKABLE QVariantMap readFile(QString path);
    Q_INVOKABLE void saveFile(QString path, QVariantMap options);

    Q_INVOKABLE QStringList filesInSceneDirectory() const;
    Q_INVOKABLE QVariantMap readSceneFile(QString filename) const;
    Q_INVOKABLE void saveSceneFile(QString filename, QVariantMap options);


    Q_INVOKABLE void setOverlayWidgetTransparent(bool state);

signals:
    void imageChanged(QString id);

private:
    void createQmlEngine();

    //QScopedPointer<EnhancedQmlApplicationEngine> qmlEngine;

    QQuickWidget* _quickWidget = nullptr;
    GraphicsView* _gw = nullptr;
    QQuickWidget* _quickWidget2 = nullptr;
    QScopedPointer<MainWindow3> _mw;
    QScopedPointer<MainWindow> _sceneView;
    WidgetAnchor* _widgetAnchor = nullptr;

    //V4L2 _videoDriver;
//    Video3* _videoDriver3;
//    Video4* _videoDriver4;
    //QImage _image;

    //QByteArray _imgPpm;
    //QStringList _info;
};
