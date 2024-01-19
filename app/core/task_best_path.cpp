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


#include "tsp_matrix.h"
#include "tsp_little_solver.h"

using MatrixD = Matrix<double>;

namespace {

QList<QPointF> blobsToScenePositions(const QList<BlobItem*>& blobs)
{
    QList<QPointF> points;
    std::transform(blobs.begin(), blobs.end(), points.begin(), [](BlobItem* blob) { return blob->scenePos(); });
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

void TaskBestPath::run()
{
    _impl->_stop = false;
    QMetaObject::invokeMethod(_impl, "run", Qt::QueuedConnection);
}

void TaskBestPath::stopProgram()
{
    _impl->_stop = true;
}

TaskBestPathPrivate::TaskBestPathPrivate()
{

}

// Скан может быть неудачным для какого то разрешения. Определить, и предупредить
void TaskBestPathPrivate::run()
{
    const auto fin = qScopeGuard([this]{ emit finished(); });

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

    // 2. Преобразовали в то координаты
    QList<QPointF> points = blobsToScenePositions(blobs);

    // 3. Добавили точку начала программы, куда уезжает каретка.
    points.push_back(QPointF(0,0));

    // 4. Преобразовали блобы в матрицу
    MatrixD distances = distanceMatrix(points);

    // 5. Подали на вход алгоритму и получили решение
    try
    {
        LittleSolver littleSolver(distances);
        QThread thread;
        littleSolver.moveToThread(&thread);
        thread.start();
        wait(100);

        connect(&littleSolver, &LittleSolver::newRecord, [](double record) { qd() << "new record is: " << record; });
        connect(&littleSolver, &LittleSolver::newSolution, [&](QList<int> solution)
        {
            QList<BlobItem*> blobsOptimized;

            // 6. Получили промежуточные элементы выстроенные по кратчайшему пути
            for (int i : solution)
                blobsOptimized.append(blobs.at(i));

            db().insert("blobs_optimized", QVariant::fromValue(blobsOptimized));
        });

        QAtomicInteger<bool> solved = false;
        connect(&littleSolver, &LittleSolver::solved, [&solved]() { solved = true; });

        QMetaObject::invokeMethod(&littleSolver, "solve", Qt::QueuedConnection);

        while (!_stop && !solved)
        {
            wait(100);
        }

        if (_stop)
        {
            littleSolver.stop = true;
            wait(100);
        }

        if (solved)
        {
            qd() << "solved";
            QList<int> finalSolution = littleSolver.finalSolution();
            // 6. Получили элементы выстроенные по кратчайшему пути

            QList<BlobItem*> blobsOptimized;

            for (int i : finalSolution)
                blobsOptimized.append(blobs.at(i));

            db().insert("blobs_optimized", QVariant::fromValue(blobsOptimized));
        }

        thread.quit();
        thread.wait(1000);
        wait(1000);
    }
    catch (...)
    {

    }

    // Тут бы надо бы найти ближайшую точки из пути к начальной точке, но пока пофигу, пусть так.



    auto finish = QDateTime::currentMSecsSinceEpoch();

    emit message("program time " + QString::number((finish - start)/1000) + " sec");
    wait(10);
}
