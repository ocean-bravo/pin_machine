#include "search_blobs.h"

#include "video4.h"
#include "serial.h"
#include "utils.h"
#include "openCv.h"
#include "data_bus.h"
#include <list>
#include <QEventLoop>
#include <QTimer>
#include <QRegExp>
#include <QMetaMethod>
#include <QScopeGuard>


namespace {

double extractFromGcodeX(QString line)
{
    return line.split(' ', Qt::SkipEmptyParts)[3].replace(QRegExp(R"( [^\d.-] )"), "").toDouble(); //G1 G90 F5000 X6 Y140
}

double extractFromGcodeY(QString line)
{
    return line.split(' ', Qt::SkipEmptyParts)[4].replace(QRegExp(R"( [^\d.-] )"), "").toDouble(); //G1 G90 F5000 X6 Y140
}

}

SearchBlobs::SearchBlobs(Video4 *video, QObject* parent)
    : QObject(parent)
    , _impl(new SearchBlobsPrivate(video))
    , _thread(new QThread)
{
    connect(_impl, &SearchBlobsPrivate::message, this, &SearchBlobs::message, Qt::QueuedConnection);

    connect(_thread.data(), &QThread::finished, _impl, &QObject::deleteLater);

    _impl->moveToThread(_thread.data());
    _thread->start();
}

void SearchBlobs::run(QString program)
{
    QMetaObject::invokeMethod(_impl, "run", Qt::QueuedConnection, Q_ARG(QString, program));
}

void SearchBlobs::stopProgram()
{
    _impl->stopProgram = true;
}



SearchBlobsPrivate::SearchBlobsPrivate(Video4 *video)
    : _video(video)
{

}


bool SearchBlobsPrivate::sendNextLine()
{
    QString line = _codeLines[_lineToSend];
    int lineNumber = _lineToSend+1;

    // Пропускаю пустые строки
    QString msg;
    if (line.length() == 0)
    {
        msg = QString("%1: skip...\n" ).arg(lineNumber);
    }
    else
    {
        _xTarget = extractFromGcodeX(line);
        _yTarget = extractFromGcodeY(line);

        //                console.log( " x target " + xTarget)
        //                console.log( " y target " + yTarget)

        serial().write(line.toLatin1() + "\n");
        msg = QString("%1: %2\n" ).arg(lineNumber).arg(line);
    }

    //appendLog(msg)
    emit message(msg);
    ++_lineToSend;

    return line.length() > 0;
}

void SearchBlobsPrivate:: startProgram()
{
    //cycle.runAsync()
}

void SearchBlobsPrivate::pauseProgram()
{
    //statusTimer.stop()
    //playPauseProgram.text = qsTr("Resume program")
}

//void SearchBlobsPrivate:: stopProgram()
//{
//    //cycle.abort()

////    statusTimer.stop()
////    playPauseProgram.checked = false
////    playPauseProgram.text = qsTr("Run program")

//    //    codeEditor.readOnly = false
//}

void SearchBlobsPrivate::waitForGetPosition(double xTarget, double yTarget)
{
    auto condition = [xTarget, yTarget]()
    {
        const QString status = db().value("status").toString();
        const double xPos = db().value("xPos").toDouble();
        const double yPos = db().value("yPos").toDouble();
        return (status == "Idle") && (std::abs(xTarget - xPos) <= 0.003) && (std::abs(yTarget - yPos) <= 0.003);
    };

    QEventLoop loop;
    QTimer::singleShot(10000, &loop, &QEventLoop::quit);

    QMetaObject::Connection conn = connect(&db(), &DataBus::valueChanged, this, [&condition, &loop](const QString& key, const QVariant&)
    {
        if ( key != "status" || key != "xPos" || key != "yPos")
            return;

        if (condition())
            loop.quit();
    });

    auto guard = qScopeGuard([=]()
    {
        disconnect(conn);
    });

    loop.exec();
}

void SearchBlobsPrivate::wait(int timeout) const
{
    if (timeout <= 0)
        return;

    waitForSignal(this, QMetaMethod::fromSignal(&SearchBlobsPrivate::interrupt), timeout);
}

void SearchBlobsPrivate::waitForSignal(const PointerToMember& signal, int timeout) const
{
    QEventLoop loop;
    QTimer::singleShot(timeout, &loop, &QEventLoop::quit);
    connect(this, signal, &loop, &QEventLoop::quit);
    loop.exec();
}

void SearchBlobsPrivate::waitForSignal(const QObject* object, const QMetaMethod& signal, int timeout) const
{
    QEventLoop loop;
    QTimer::singleShot(timeout, &loop, &QEventLoop::quit);

    const int index = loop.metaObject()->indexOfMethod(QMetaObject::normalizedSignature("quit()"));
    const QMetaMethod quitMetaMethod = loop.metaObject()->method(index);

    connect(object, signal, &loop, quitMetaMethod);
    loop.exec();
}


void SearchBlobsPrivate::run(QString program)
{
    _lineToSend = 0;
    _codeLines = program.split("\n", Qt::KeepEmptyParts);

    QTimer statusTimer;
    connect(&statusTimer, &QTimer::timeout, this, []() { serial().write("?\n"); });
    statusTimer.start(100);

    db().insert("capture_number", 0);
    //ImagesStorage.clearCaptured()
    opencv().resetFoundBlobs();

    wait(200);


    while (true)
    {
        if (stopProgram)
        {
            emit message("program interrupted\n");
            break;
        }

        if (sendNextLine()) { // Если строка пустая, никаких действий после нее не надо делать

            waitForGetPosition(_xTarget, _yTarget);

            emit message("capturing ...\n");
            //let a = Date.now()
            _video->capture();
            waitForSignal(_video, QMetaMethod::fromSignal(&Video4::captured), 2000);
            //let b = Date.now()
            //emit message("captured " + String(b-a) + "ms\n");

            // Дать обработаться захвату, получить номер capture_number и потом его инкрементировать
            wait(1);
            db().insert("capture_number", db().value("capture_number").toInt() + 1);
        }

        if (_lineToSend >= _codeLines.length())
        {
            emit message("program finished\n");
            break;
        }
    }
}


