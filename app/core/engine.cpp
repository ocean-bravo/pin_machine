#include "engine.h"

#include "serial.h"
#include "version.h" // автогенерированный файл qmake из version.h.in
#include "utils.h"

#include <QCoreApplication>
#include <QQmlContext>
#include <QApplication>
#include <QQmlApplicationEngine>


Engine::Engine(QObject* parent)
    : QObject(parent)
    , _qmlEngine(nullptr)
{
    _serial.reset(new Serial);

    createQmlEngine();
}

Engine::~Engine()
{
    _qmlEngine.reset();
}

void Engine::createQmlEngine()
{
    _qmlEngine.reset(new QQmlApplicationEngine());

    _qmlEngine->addImportPath(appDir() + "libs");

    _qmlEngine->rootContext()->setContextProperty("Serial", _serial.data());
    _qmlEngine->load(QUrl::fromLocalFile(appDir() + QString("gui/main.qml")));
}

