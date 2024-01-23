#include "task_best_path.h"
#include "wait.h"
#include "video4.h"
#include "serial.h"
#include "utils.h"
#include "utils2.h"
#include "data_bus.h"
#include "scene.h"
#include "graphics_view.h"

#include <QEventLoop>
#include <QTimer>
#include <QMetaMethod>
#include <QScopeGuard>
#include <QDateTime>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>

#include "openCv.h"
#include "settings.h"
#include "common.h"
#include "blob_item.h"

#include <valgrind/callgrind.h>


#include "tsp_matrix.h"
#include "tsp_little_solver.h"

using MatrixD = Matrix<double>;

namespace {

QList<QPointF> blobsToScenePositions(const QList<BlobItem*>& blobs)
{
    QList<QPointF> points;
    for (BlobItem* blob : blobs)
        points.push_back(blob->scenePos());
    return points;
}

MatrixD distanceMatrix(const QList<QPointF>& positions)
{
    // создание матрицы размерности количества вершин
    MatrixD distances(positions.size());
    MatrixD::Size i = 0;
    MatrixD::Size j = 0;

    for (QPointF p1 : positions)
    {
        j = 0;
        for (QPointF p2 : positions)
        {
            // подсчет расстояний
            distances(i, j) = sqrt(pow(p1.x() - p2.x(), 2) + pow(p1.y() - p2.y(), 2));
            ++j;
        }
        ++i;
    }

    return distances;
}

// nonoptimizedPointCoords - координаты блобов для забивки, с случайном порядке, как получены со сцены
// solution - решение, получено от алгоритма Литтла. Упорядоченный список индексов узказывающих на nonoptimizedPointCoords
// Прокручиваю по кругу точки, пока первой точкой не будет точка после координат начала. Стартовой точки не будет в выходном пути
QList<QPointF> solutionToPath(const QList<QPointF>& nonoptimizedPointCoords, const QList<int>& solution, QPointF startPoint)
{
    QList<QPointF> optimizedPointCoords;
    for (int i : solution)
        optimizedPointCoords.append(nonoptimizedPointCoords.at(i));

    // Прокручиваю по кругу точки - беру первую; проверяю, ровна ли она стартовой; если нет, запихиваю в конец; если да - готово
    for (int i = 0; i < optimizedPointCoords.size(); ++i)
    {
        QPointF point = optimizedPointCoords.takeFirst();
        optimizedPointCoords.push_back(point);

        if (point == startPoint)
        {
            optimizedPointCoords.push_front(point);
            break;
        }
    }

    return optimizedPointCoords;
}

}

TaskBestPath::TaskBestPath(QObject* parent)
    : QObject(parent)
    , _impl(new TaskBestPathPrivate)
    , _thread(new QThread)
{
    connect(_impl, &TaskBestPathPrivate::message, this, &TaskBestPath::message, Qt::QueuedConnection);
    connect(_impl, &TaskBestPathPrivate::finished, this, &TaskBestPath::finished, Qt::QueuedConnection);

    connect(_thread.data(), &QThread::finished, _impl, &QObject::deleteLater);

    _impl->moveToThread(_thread.data());
    _thread->start();
}

TaskBestPath::~TaskBestPath()
{
    _thread->quit();
    _thread->wait(1000);
}

void TaskBestPath::run(QPointF pos)
{
    _impl->_stop = false;
    QMetaObject::invokeMethod(_impl, "run", Qt::QueuedConnection, Q_ARG(QPointF, pos));
}

void TaskBestPath::stopProgram()
{
    qd() << "stop program";
    _impl->_stop = true;
}

bool TaskBestPath::isRunning() const
{
    return _impl->_running;
}


TaskBestPathPrivate::TaskBestPathPrivate()
{

}

// Скан может быть неудачным для какого то разрешения. Определить, и предупредить
void TaskBestPathPrivate::run(QPointF startPoint)
{
    qd() << "run program";
    _running = true;
    auto running = qScopeGuard([this]{ _running = false; });

    const auto fin = qScopeGuard([this]
    {
        emit finished();
        qd() << "finished program";

    });

    if (!_mutex.tryLock()) return;
    auto mutexUnlock = qScopeGuard([this]{ _mutex.unlock(); });

    const auto start = QDateTime::currentMSecsSinceEpoch();

    // 1. Получили все блобы для забивки
    QList<BlobItem*> blobs;

    every<BlobItem>(scene().items(), [&blobs](BlobItem* blob)
    {
        if (blob->isPunch())
            blobs.push_back(blob);
    });

    if (blobs.isEmpty())
        return;


    // 2. Преобразовали в координаты блобов
    QList<QPointF> coords = blobsToScenePositions(blobs);

    // 3. Добавили точку начала программы, куда уезжает каретка.
    coords.push_back(startPoint);

    // 4. Преобразовали блобы в матрицу
    MatrixD distances = distanceMatrix(coords);

    // 5. Подали на вход алгоритму и получили решение

    try
    {
        CALLGRIND_START_INSTRUMENTATION;
        CALLGRIND_TOGGLE_COLLECT;
        qd() << "start";
        LittleSolver littleSolver(distances);
        QThread thread(this);
        littleSolver.moveToThread(&thread);
        thread.start();
        //wait(100);

        connect(&thread, &QThread::started, &littleSolver, &LittleSolver::solve);
        connect(&littleSolver, &LittleSolver::newRecord, [](double record) { qd() << "new record is: " << record; });
        connect(&littleSolver, &LittleSolver::newSolution, [&](QList<int> solution)
        {
            // 6. Получили промежуточные элементы выстроенные по кратчайшему пути
            QList<QPointF> coordsOptimized = solutionToPath(coords, solution, startPoint);
            db().insert("path_optimized", QVariant::fromValue(coordsOptimized));
        });

        QAtomicInteger<bool> solved = false;
        connect(&littleSolver, &LittleSolver::solved, [&solved]()
        {
            qd() << "solved called";
            solved = true;
        });

        //QMetaObject::invokeMethod(&littleSolver, "solve", Qt::QueuedConnection);

        while (!_stop && !solved)
        {
            CALLGRIND_STOP_INSTRUMENTATION;
            wait(100);
            CALLGRIND_START_INSTRUMENTATION;
        }

        if (_stop)
        {
            qd() << "stopped";
            littleSolver.stop = true;
            CALLGRIND_STOP_INSTRUMENTATION;
            wait(100);
            CALLGRIND_START_INSTRUMENTATION;
        }

        if (solved)
        {
            qd() << "solved";

            // 6. Получили координаты точек выстроенных по кратчайшему пути между ними
            QList<int> finalSolution = littleSolver.finalSolution();
            QList<QPointF> coordsOptimized = solutionToPath(coords, finalSolution, startPoint);
            db().insert("path_optimized", QVariant::fromValue(coordsOptimized));
        }
        qd() << "stop";
        CALLGRIND_TOGGLE_COLLECT;
        CALLGRIND_STOP_INSTRUMENTATION;
        thread.quit();
        thread.wait(1000);
        wait(100);
    }
    catch (...)
    {

    }

    CALLGRIND_DUMP_STATS;

    auto finish = QDateTime::currentMSecsSinceEpoch();

    emit message("program time " + QString::number((finish - start)/1000) + " sec");
    wait(10);
}
