#include "task_scan.h"
#include "wait.h"
#include "video4.h"
#include "serial.h"
#include "utils.h"
#include "utils2.h"
#include "openCv.h"
#include "data_bus.h"
#include "scene.h"
#include "blob_item.h"
#include "graphics_view.h"

#include <QEventLoop>
#include <QTimer>
#include <QRegExp>
#include <QMetaMethod>
#include <QScopeGuard>
#include <QDateTime>

namespace {

}

TaskScan::TaskScan(QObject* parent)
    : QObject(parent)
    , _impl(new TaskScanPrivate)
    , _thread(new QThread)
{
    connect(_impl, &TaskScanPrivate::message, this, &TaskScan::message, Qt::QueuedConnection);
    connect(_impl, &TaskScanPrivate::finished, this, &TaskScan::finished, Qt::QueuedConnection);

    connect(_thread.data(), &QThread::finished, _impl, &QObject::deleteLater);

    _impl->moveToThread(_thread.data());
    _thread->start();
}

TaskScan::~TaskScan()
{
    _thread->quit();
    _thread->wait(1000);
}

void TaskScan::run(QString program, int width, int height, QString fourcc)
{
    _impl->_stop = false;
    QMetaObject::invokeMethod(_impl, "run", Qt::QueuedConnection,
                              Q_ARG(QString, program), Q_ARG(int, width), Q_ARG(int, height), Q_ARG(QString, fourcc));
}

void TaskScan::stopProgram()
{
    _impl->_stop = true;
}


TaskScanPrivate::TaskScanPrivate()
{

}

bool TaskScanPrivate::sendNextLine()
{
    QString line = _codeLines[_lineToSend];
    int lineNumber = _lineToSend+1;

    // Пропускаю пустые строки
    QString msg;
    if (line.length() == 0)
    {
        msg = QString("%1: skip..." ).arg(lineNumber);
    }
    else
    {
        _xTarget = extractFromGcodeX(line);
        _yTarget = extractFromGcodeY(line);

        serial().write(line.toLatin1() + "\n");
        msg = QString("%1: %2" ).arg(lineNumber).arg(line);
    }

    emit message(msg);
    ++_lineToSend;

    return line.length() > 0;
}

void TaskScanPrivate::pauseProgram()
{

}

//void SearchBlobsPrivate::waitForGetPosition(double xTarget, double yTarget)
//{
//    auto condition = [xTarget, yTarget]() -> bool
//    {
//        const QString status = db().value("status").toString();
//        const double xPos = db().value("xPos").toDouble();
//        const double yPos = db().value("yPos").toDouble();

//        return (status == "Idle") && (std::abs(xTarget - xPos) <= 0.003) && (std::abs(yTarget - yPos) <= 0.003);
//    };

//    QEventLoop loop;
//    QTimer::singleShot(10000, &loop, &QEventLoop::quit);

//    QMetaObject::Connection conn = connect(&db(), &DataBus::valueChanged, this, [&condition, &loop](const QString& key, const QVariant&)
//    {
//        if ( key == "status" || key == "xPos" || key == "yPos")
//        {
//            if (condition())
//                loop.quit();
//        }
//    });

//    auto guard = qScopeGuard([=]()
//    {
//        disconnect(conn);
//    });

//    // у машины есть зона нечувствительности, т.е. она не реагирует на микроперемещения, около 0,002 мм
//    if (condition())
//        return;

//    loop.exec();
//}

void TaskScanPrivate::run(QString program, int width, int height, QString fourcc)
{
    const auto fin = qScopeGuard([this]{ emit finished(); });

    if (!_mutex.tryLock()) return;
    auto mutexUnlock = qScopeGuard([this]{ _mutex.unlock(); });

    QTimer statusTimer;
    connect(&statusTimer, &QTimer::timeout, this, []() { serial().write("?\n"); });
    statusTimer.start(100);

    const auto start = QDateTime::currentMSecsSinceEpoch();

    // Мешается GUI. При обнуражении камеры идет ее запуск. Решить бы это как то.
    video().reloadDevices();
    wait(500);
    video().stop();

    db().insert("resolution_width", width);
    db().insert("resolution_height", height);

    video().changeCamera(cameraId(), width, height, fourcc);
    video().start();

    _lineToSend = 0;
    _codeLines = program.split("\n", Qt::KeepEmptyParts);

    db().insert("capture_number", 0);
    //ImagesStorage.clearCaptured()
    scene().clear();
    scene().addBoard();

    // Костыль. Чтобы плата показывалась во весь экран. Иначе плата была неизвестно где, на сцене ее было не видно.
    for (QGraphicsView* view : scene().views())
    {
        GraphicsView* v = dynamic_cast<GraphicsView*>(view);
        if (v)
            v->fit();
    }

//    scene().addBlob(5,5,25);

//    scene().addBlob(35,5,25);
//    {
//        BlobItem* bl = scene().addBlob(5,35,2);
//        bl->setFiducial(true);
//        //bl->setRotation(45);
//    }
//    {
//        BlobItem* bl = scene().addBlob(5.05,35.05,2);
//        bl->setRealFiducial(true);
//        //bl->setRotation(0);
//    }

//    return;

    wait(200);

    auto connection = connect(&video(), &Video4::captured, this, [](QImage img)
    {
        scene().setImage(img); // копия не нужна. Внутри делается копия
        db().insert("image_raw_captured", img.copy());
        opencv().appendToBlobDetectorQueue(img.copy());
    });
    auto guard = qScopeGuard([=]() { disconnect(connection); });

    while (true)
    {
        if (_stop)
        {
            emit message("program interrupted");
            break;
        }

        if (sendNextLine()) { // Если строка пустая, никаких действий после нее не надо делать

            waitPosXY(QPointF(_xTarget, _yTarget));

            emit message("capturing ...");
            auto a = QDateTime::currentMSecsSinceEpoch();
            video().capture();
            waitForSignal(&video(), &Video4::captured, 2000);
            auto b = QDateTime::currentMSecsSinceEpoch();
            emit message(QString("captured %1 ms").arg(b-a));

            // Дать обработаться захвату, получить номер capture_number и потом его инкрементировать
            wait(1);
            db().insert("capture_number", db().value("capture_number").toInt() + 1);
        }

        if (_lineToSend >= _codeLines.length())
        {
            emit message("program finished");
            break;
        }
    }

    wait(300); // не успевает блоб отдетектироваться
    scene().removeDuplicatedBlobs();

    auto finish = QDateTime::currentMSecsSinceEpoch();

    emit message("program time " + QString::number((finish - start)/1000) + " sec");
    wait(10);
}
