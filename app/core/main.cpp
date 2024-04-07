#include "engine.h"
#include "utils.h"
#include "log/logger.h"

#include <QtDebug>
#include <QSysInfo>
#include <QDateTime>
#include <QApplication>
#include <QScopedPointer>
#include <QScreen>
#include <QQmlApplicationEngine>

#include <iostream>
#include <exception>      // std::set_terminate


//#include "mainwindow2.h"
#include <QApplication>

#include "mainwindow.h"

void myMessageOutput(QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
    switch (type)
    {
        case QtDebugMsg:
            std::cerr << QString("Debug: %1 (%2:%3, %4)\n").arg(msg).arg(context.file).arg(context.line).arg(
                          context.function).toStdString();
            break;
        case QtWarningMsg:
            std::cerr << QString("Warning: %1 (%2:%3, %4)\n").arg(msg).arg(context.file).arg(context.line).arg(
                          context.function).toStdString();
            break;
        case QtCriticalMsg:
            std::cerr << QString("Critical: %1 (%2:%3, %4)\n").arg(msg).arg(context.file).arg(context.line).arg(
                          context.function).toStdString();
            break;
        case QtFatalMsg:
            std::cerr << QString("Fatal: %1 (%2:%3, %4)\n").arg(msg).arg(context.file).arg(context.line).arg(
                          context.function).toStdString();
            abort();
        default:
            break;
    }
}

void myterminate()
{
    std::cerr << "terminate handler called\n";
    abort();  // forces abnormal termination
}

#include <QSplashScreen>
#include <QQuickWindow>
// TODO: если блоб фидушифл, не давать быть реал фидушиал и наоборот
int main(int argc, char* argv[])
{
    //std::set_terminate(myterminate);

    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication app(argc, argv);

    Logger::instance();

    QEventLoop loop;
    QTimer::singleShot(1000, &loop, &QEventLoop::quit);
    QObject::connect(&Logger::instance(), &Logger::inited, &loop, &QEventLoop::quit);
    loop.exec();

    QScopedPointer<Engine> engine(new Engine());

    // Engine должен быть удален до разрушения главного цикла обработки событий.
    QObject::connect(&app, &QApplication::aboutToQuit, engine.data(), [&engine]() { engine.reset(); });

// Test 1
    //MainWindow w;
    //w.show();

    // MainWindow scene;

    // QRect screenrect = app.primaryScreen()->geometry();
    // scene.resize(1920 / 2, screenrect.height());
    // scene.move(1920/2, screenrect.top());
    // scene.show();

    return app.exec();
}
