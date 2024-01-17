#pragma once

#include <QObject>

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
    template<typename T>
    using Vector = std::vector<T>;

    template<typename T>
    using List = std::list<T>;

    template<typename T1, typename T2>
    using Pair = std::pair<T1, T2>;

    using Size = size_t;

    // список вершин как список пар номеров смежных вершин
    using arclist = List<Pair<Size, Size>>;
    using MatrixD = Matrix<double>;
    using MatrixPtr = std::unique_ptr<MatrixD>;

    // на вход матрица расстояний и требуемая верхняя граница
    LittleSolver(const MatrixD &m, double record = DBL_MAX);
    ~LittleSolver();

    // получить решение
    List<Size> solution() const;

    // было ли найдето решение, не превышающее заданную границу
    bool isSolved() const;

    // не должен быть скопирован
    LittleSolver(const LittleSolver&) = delete;
    LittleSolver &operator=(const LittleSolver&) = delete;

public slots:
    // основной метод
    void solve();

signals:
    void newRecord(double);
    void newSolution(List<Pair<Size, Size>>);
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
    List<Pair<Size, Size>>  findBestZeros(const MatrixD &matrix) const;
    // получение коэффициента для элемента (r, c)
    // r - row; c - column
    static double getCoefficient(const MatrixD &m, Size r, Size c);
    // записать последний проверенный путь
    void logPath(const arclist &path);

    // исходная матрица расстояний
    MatrixPtr _sourceMatrix;
    // рекорд, он же длина лучшего пути, он же верхняя граница
    std::atomic<double> _record;
    // лучшее решение
    arclist _arcs;
    // итоговое решение
    List<Size> _solution;
    // последний просмотренный список ребер
    arclist _lastStep;
    // для доступа к промежуточным результатам из другого потока
    mutable std::mutex _mutex;
    // значение, принимаемое за бесконечность
    double _infinity;
};
