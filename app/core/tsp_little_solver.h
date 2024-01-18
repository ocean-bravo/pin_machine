#pragma once

#include <QList>
#include <QObject>
#include <QAtomicInteger>

#include <list>
#include <vector>
#include <utility>
#include <memory>
#include <cfloat>
#include <mutex>
#include <atomic>

template<typename T>
class Matrix;

class LittleSolver : public QObject
{
    Q_OBJECT

public:
    // список вершин как список пар номеров смежных вершин
    using arclist = std::list<std::pair<size_t, size_t>>;
    using MatrixD = Matrix<double>;
    using MatrixPtr = std::unique_ptr<MatrixD>;

    // на вход матрица расстояний и требуемая верхняя граница
    LittleSolver(const MatrixD &m, double record = DBL_MAX);
    ~LittleSolver();

    // получить решение
    QList<int> finalSolution() const;

    // было ли найдето решение, не превышающее заданную границу
    bool isSolved() const;

    // не должен быть скопирован
    LittleSolver(const LittleSolver&) = delete;
    LittleSolver &operator=(const LittleSolver&) = delete;

    QAtomicInteger<bool> stop = false;


public slots:
    // основной метод
    void solve();

signals:
    void newRecord(double);
    void newSolution(QList<int>);
    void solved();

private:
    // основная рекурсивная функция поиска пути
    // m - текущая матрица стоимостей
    // arcs - текущий найденный путь
    // bottomLimit - текущая нижняя граница
    void handleMatrix(const MatrixD &m, const arclist &arcs, double bottomLimit);
    // скммарная длина набора ребер
    double cost(const arclist &arcs) const;
    // сравнить предложенное решение с оптимальным
    void candidateSolution(const arclist &arcs);
    // добавление недостающей бесконечности в матрицу
    // стоимостей для избежания преждевременных циклов
    void addInfinity(MatrixD &m);
    // произвести ряд вычитаний, чтобы в каждой строке
    // и каждом столбце были бесконечности.
    // возвращает значение, на которое увеличится нижняя граница
    double subtractFromMatrix(MatrixD &m) const;
    // поиск нулевых коэффициентов с максимальными коэффициентами
    std::list<std::pair<size_t, size_t>>  findBestZeros(const MatrixD &matrix) const;
    // получение коэффициента для элемента (r, c)
    // r - row; c - column
    static double getCoefficient(const MatrixD &m, size_t r, size_t c);

    // исходная матрица расстояний
    MatrixPtr _sourceMatrix;
    // рекорд, он же длина лучшего пути, он же верхняя граница
    std::atomic<double> _record;
    // лучшее решение
    arclist _arcs;
    // итоговое решение
    std::list<size_t> _solution;
    // последний просмотренный список ребер
    //arclist _lastStep;
    // для доступа к промежуточным результатам из другого потока
    mutable std::mutex _mutex;
    // значение, принимаемое за бесконечность
    double _infinity;
};
