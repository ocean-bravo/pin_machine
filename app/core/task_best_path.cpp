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

MatrixD distanceMatrix(const MatrixD::List<BlobItem*>& blobs)
{
    // создание матрицы размерности количества вершин
    MatrixD distances(blobs.size());
    MatrixD::Size i = 0;
    MatrixD::Size j = 0;

    for (BlobItem* b1 : blobs)
    {
        j = 0;
        for (BlobItem* b2 : blobs)
        {
            // подсчет расстояний
            distances(i, j) = sqrt(pow(b1->x() - b2->x(), 2) + pow(b1->y() - b2->y(), 2));
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
    MatrixD::List<BlobItem*> blobs;

    every<BlobItem>(scene().items(), [&blobs](BlobItem* blob)
    {
        if (blob->isPunch())
            blobs.push_back(blob);
    });

    // 2. Преобразовали блобы в матрицу
    MatrixD distances = distanceMatrix(blobs);

    // 3. Подали на вход алгоритму и получили решение
    MatrixD::List<MatrixD::Size> solution;

    try
    {
        LittleSolver littleSolver(distances);
        QThread thread;
        littleSolver.moveToThread(&thread);
        thread.start();
        wait(300);

        connect(&littleSolver, &LittleSolver::newRecord, [](double record) { qd() << "new record is: " << record; });
        connect(&littleSolver, &LittleSolver::newSolution, [](MatrixD::List<MatrixD::Pair<MatrixD::Size, MatrixD::Size>> solution) { /*qd() << "new solution is: " << solution;*/ });

        QAtomicInteger<bool> solved = false;
        connect(&littleSolver, &LittleSolver::solved, [&solved]() { solved = true; });

        QMetaObject::invokeMethod(&littleSolver, "solve", Qt::QueuedConnection);

        while (!_stop || !solved)
        {
            wait(100);
        }

        if (solved)
        {
            qd() << "solved";
            solution = littleSolver.solution();
        }

        thread.quit();
        wait(1000);
    }
    catch (...)
    {

    }

    // 4. Получили элементы выстроенные по кратчайшему пути
    // QList<BlobItem*> blobsOptimized;
    // for (size_t i : solution)
    //     blobsOptimized.append(blobs.at(i));



    // QList<QPair<int, int>> arclist;
    // QList<int> points;

    // points.push_back(arclist.first().first);
    // int last = arclist.first().second;

    // while (arclist.size() > 0)
    // {
    //     for (int i = 0; i < arclist.size(); ++i)
    //     {
    //         QPair<int, int> next = arclist[i];

    //         if (next.first == last)
    //         {
    //             points.push_back(next.second);
    //             last = next.second;
    //             arclist.removeAt(i);
    //             break;
    //         }
    //     }
    // }





    // Тут бы надо бы найти ближайшую точки из пути к начальной точке, но пока пофигу, пусть так.

    //db().insert("blobs_optimized", QVariant::fromValue(blobsOptimized));

    auto finish = QDateTime::currentMSecsSinceEpoch();

    emit message("program time " + QString::number((finish - start)/1000) + " sec");
    wait(10);
}
