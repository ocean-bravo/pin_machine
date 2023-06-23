#pragma once

#include <QObject>
#include <QScopedPointer>

class QQmlApplicationEngine;
class Serial;

class Engine : public QObject
{
    Q_OBJECT

public:
    explicit Engine(QObject* parent = nullptr);
    ~Engine();

private:
    void createQmlEngine();

    QScopedPointer<QQmlApplicationEngine> _qmlEngine;
    QScopedPointer<Serial> _serial;
};
