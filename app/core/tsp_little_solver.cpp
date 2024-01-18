#include "tsp_little_solver.h"
#include "tsp_matrix.h"

#include "utils.h"

#include <list>
#include <utility>

template<typename T>
using List = std::list<T>;

template<typename T1, typename T2>
using Pair = std::pair<T1, T2>;

using Size = size_t;

using std::mutex;
using std::lock_guard;

namespace {

// LittleSolver::arclist listToPairs(const List<Size>& l)
// {
//     LittleSolver::arclist p;

//     if (l.empty())
//         return p;

//     auto iter1 = l.cbegin();
//     auto iter2 = l.cbegin();
//     ++iter2;
//     while (iter2 != l.cend())
//     {
//         p.push_back(Pair<Size, Size>(*iter1, *iter2));
//         ++iter1;
//         ++iter2;
//     }
//     return p;
// }

}


LittleSolver::LittleSolver(const MatrixD& m, double record)
    : _record(record)
    , _infinity(0)
{
    _sourceMatrix = std::make_unique<MatrixD>(m);
    // сумма всех элементов матрицы выступает в качестве бесконечности
    for (Size i = 0; i < m.size(); i++)
        for (Size j = i + 1; j < m.size(); j++)
            _infinity += (m.item(i, j) + m.item(j, i));
    // заполнение диагонали бесконечностями
    for (Size i = 0; i < _sourceMatrix->size(); i++)
        _sourceMatrix->item(i, i) = _infinity;
}

LittleSolver::~LittleSolver()
{}

void LittleSolver::solve()
{
    // решение
    handleMatrix(*_sourceMatrix, arclist(), 0);

    if (stop)
        return;

    // запись решения
    // доюавление нулевой вершины как начальной
    _solution.push_back(0);
    // посик следующей вершины
    while (!_arcs.empty())
    {
        auto iter = _arcs.begin();
        while (iter != _arcs.end())
        {
            // если есть ребро, исходящее из последней вершины
            // добавление в решение смежной вершины
            // и удаление этого ребра из списка
            if (iter->first == _solution.back())
            {
                _solution.push_back(iter->second);
                iter = _arcs.erase(iter);
            }
            else
                ++iter;
        }
    }

    emit solved();
}

List<Size> LittleSolver::solution() const
{
    return _solution;
}

bool LittleSolver::isSolved() const
{
    return _solution.size() != 1;
}

void LittleSolver::handleMatrix(const MatrixD &m, const arclist &path, double bottomLimit)
{
    if (stop)
        return;

    if (m.size() < 2)
        throw std::logic_error("Matrix smaller than 2x2");

    // если матрица меньше 2, решение заканчивается
    if (m.size() == 2)
    {
        // записать текущий путь как последний рассмотренный
        logPath(path);
        // выбор элемента, не равного бесконечности, в первой строке
        Size i = m.item(0, 0) >= _infinity ? 1 : 0;
        // создание списка с результирующим путем
        arclist result(path);
        // добавление индексов элементов, не равных бесконечности
        result.push_back(Pair<Size, Size>(m.rowIndex(0), m.columnIndex(i)));
        result.push_back(Pair<Size, Size>(m.rowIndex(1), m.columnIndex(1 - i)));
        // сравнение пути с минимальным
        candidateSolution(result);
        return;
    }


    // создается копия переданной матрицы, т.к. он константна
    MatrixD matrix(m);
    // вычитание минимальных элементов строк и столбцов
    // увеличение нижней границы
    bottomLimit += subtractFromMatrix(matrix);
    // сравнение верхней и нижней границ
    if (bottomLimit > _record)
    {
        // записать текущий путь как последний рассмотренный
        logPath(path);
        return;
    }

    // получение списка нулевых элементов с максимальными коэффициентами
    // на самом деле достаточно одного
    auto zeros = findBestZeros(matrix);

    // переход к множествам, содержащим и не содержащим ребро edge
    auto edge = zeros.front();
    // копия матрицы
    auto newMatrix(matrix);
    //     из матрицы удаляются строка и столбец, соответствующие вершинам ребра
    newMatrix.removeRowColumn(edge.first, edge.second);
    //     ребро iter добавляется к пути
    auto newPath(path);
    newPath.push_back(Pair<Size, Size>(matrix.rowIndex(edge.first), matrix.columnIndex(edge.second)));
    // добавление бесконечности для избежания преждевремнного цикла
    addInfinity(newMatrix);
    // обработка множества, содержащего ребро edge
    handleMatrix(newMatrix, newPath, bottomLimit);

    if (stop)
        return;

    // переход к множеству, не сожержащему ребро edge
    // снова копирование матрицы текущего шага
    newMatrix = matrix;
    // добавление бесконечности на место iter
    newMatrix(edge.first, edge.second) = _infinity + 1;
    // обработка множества, не сожержащего ребро edge
    handleMatrix(newMatrix, path, bottomLimit);

    if (stop)
        return;
}

double LittleSolver::cost(const arclist &arcs) const
{
    // инициализация нулем
    double result(0);
    for (auto &iter : arcs)
        // суммирование элементов исходной матрицы, соответствующих ребрам
        result += _sourceMatrix->item(iter.first, iter.second);

    return result;
}

void LittleSolver::candidateSolution(const arclist &arcs)
{
    double curCost;
    // сравнение рекорда со стоимостью текущего пути
    if (_record < (curCost = cost(arcs)))
    {
        return;
    }
    std::lock_guard<std::mutex> g(_mutex);
    // копирование стоимости и пути
    _record = curCost;

    emit newRecord(_record);
    emit newSolution(arcs);
    _arcs = arcs;
}

void LittleSolver::addInfinity(MatrixD &m)
{
    // массивы с информацией о том, в каких столбцах и строках содержится бесконечность
    Vector<bool> infRow(m.size(), false), infColumn(m.size(), false);
    // обход всей матрицы
    for (Size i = 0; i < m.size(); i++)
        for (Size j = 0; j < m.size(); j++)
            if (m.item(i, j) == _infinity)
            {
                infRow[i] = true;
                infColumn[j] = true;
            }
    // поиск строки, не содержащей бесконечности
    Size notInf;
    for (Size i = 0; i < infRow.size(); i++)
        if (!infRow[i])
        {
            notInf = i;
            break;
        }

    // поиск столбца, не содаржащего бесконечности и добавление бесконечности
    for (Size j = 0; j < infColumn.size(); j++)
        if (!infColumn[j])
        {
            m.item(notInf, j) = _infinity;
            break;
        }
}

double LittleSolver::subtractFromMatrix(MatrixD &m) const
{
    // сумма всех вычтенных значений
    double substractSum = 0;
    // массивы с минимальными элементами строк и столбцов
    Vector<double> minRow(m.size(), DBL_MAX), minColumn(m.size(), DBL_MAX);
    // обход всей матрицы
    for (Size i = 0; i < m.size(); ++i)
    {
        for (Size j = 0; j < m.size(); ++j)
            // поиск минимального элемента в строке
            if (m(i, j) < minRow[i])
                minRow[i] = m(i, j);

        for (Size j = 0; j < m.size(); ++j)
        {
            // вычитание минимальных элементов из всех
            // элементов строки кроме бесконечностей
            if (m(i, j) < _infinity) {
                m(i, j) -= minRow[i];
            }
            // поиск минимального элемента в столбце после вычитания строк
            if ((m(i, j) < minColumn[j]))
                minColumn[j] = m(i, j);
        }
    }

    // вычитание минимальных элементов из всех
    // элементов столбца кроме бесконечностей
    for (Size j = 0; j < m.size(); ++j)
        for (Size i = 0; i < m.size(); ++i)
            if (m(i, j) < _infinity)
            {
                m(i, j) -= minColumn[j];
            }

    // суммирование вычтенных значений
    for (auto i : minRow)
        substractSum += i;

    for (auto i : minColumn)
        substractSum += i;

    return substractSum;
}

List<Pair<Size, Size>> LittleSolver::findBestZeros(const MatrixD &matrix) const
{
    // список координат нулевых элементов
    List<Pair<Size, Size>> zeros;
    // список их коэффициентов
    List<double> coeffList;

    // максимальный коэффициент
    double maxCoeff = 0;
    // поиск нулевых элементов
    for (Size i = 0; i < matrix.size(); ++i)
        for (Size j = 0; j < matrix.size(); ++j)
            // если равен нулю
            if (!matrix(i, j))
            {
                // добавление в список координат
                zeros.push_back(Pair<Size, Size>(i, j));
                // расчет коэффициена и добавление в список
                coeffList.push_back(getCoefficient(matrix, i, j));
                // сравнение с максимальным
                maxCoeff = std::max(maxCoeff, coeffList.back());
            }
    { // область видимости итераторов
        auto zIter = zeros.begin();
        auto cIter = coeffList.begin();
        while (zIter != zeros.end())
        {
            if (*cIter != maxCoeff)
            {
                // если коэффициент не максимальный, удаление его из списка
                zIter = zeros.erase(zIter);
                cIter = coeffList.erase(cIter);
            }
            else
            {
                ++zIter;
                ++cIter;
            }
        }
    }

    return zeros;
}

double LittleSolver::getCoefficient(const MatrixD &m, Size r, Size c)
{
    double rmin, cmin;
    rmin = cmin = DBL_MAX;
    // обход строки и столбца
    for (Size i = 0; i < m.size(); ++i)
    {
        if (i != r)
            rmin = std::min(rmin, m(i, c));
        if (i != c)
            cmin = std::min(cmin, m(r, i));
    }

    return rmin + cmin;
}

void LittleSolver::logPath(const LittleSolver::arclist &path)
{
    std::lock_guard<std::mutex> g(_mutex);
    _lastStep = path;
}
