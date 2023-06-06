#include "engine.h"

#include "serial.h"
#include "version.h" // автогенерированный файл qmake из version.h.in
#include "utils.h"


#include <QCoreApplication>
#include <QQmlContext>
#include <QDir>
#include <QDebug>

#include <QApplication>
#include <QQmlApplicationEngine>
#include <QtQml>
#include <QtDebug>
#include <QCursor>
#include <QSysInfo>
#include <QScopedPointer>
#include <QtConcurrent>

namespace {

}

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

// Ищет порт c устройством на котором содержится "FTDI" в поле "производтель".
void Engine::findUsbSerialPort()
{
    // Костыль, для возможного открытия виртуальных портов, которые не обнаруживаются через QSerialPortInfo::availablePorts()
    // Задаю жестко имя порта через настройки

//    Settings& settings = Settings::instance();
//    const QString portName =settings.value("common/port_name").toString();

//    if (!portName.isEmpty())
//    {
//        _usbSerialPortName = portName;
//        emit usbPortFound();
//        _serialPortUpdateTimer.reset(); // удаляется таймер, чтобы не занимать поток, он больше не нужен.
//        return;
//    }

//    for (QSerialPortInfo& info : QSerialPortInfo::availablePorts())
//    {
//        if (info.manufacturer().contains("FTDI"))
//        {
//            _usbSerialPortName = info.portName();
//            emit usbPortFound();
//            _serialPortUpdateTimer.reset(); // удаляется таймер, чтобы не занимать поток, он больше не нужен.
//            return;
//        }
//    }
//    _serialPortUpdateTimer->start(); // для старта таймера из отдельного потока и был создан Timer
}

void Engine::createQmlEngine()
{
    _qmlEngine.reset(new QQmlApplicationEngine());
    _qmlEngine->rootContext()->setContextProperty("Serial", _serial.data());
    _qmlEngine->load(QUrl::fromLocalFile(appDir() + QString("gui/main.qml")));
}

