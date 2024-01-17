#pragma once

#include <vector>

template<typename ItemType>
class Matrix
{
public:
    template<typename T>
    using Vector = std::vector<T>;

    template<typename T>
    using List = std::list<T>;

    template<typename T1, typename T2>
    using Pair = std::pair<T1, T2>;

    using Size = size_t;

    explicit Matrix(Size size = 0, ItemType values = 0);

    // доступ к элементам по индексам
    const ItemType &item(Size row, Size column) const;
    ItemType &item(Size row, Size column);
    const ItemType &operator()(Size row, Size column) const;
    ItemType &operator()(Size row, Size column);

    // получение "реальных" идексов
    // e.g. если была удалена строка 1 rowIndex(1) == 2
    // ?необходимо для алгоритма Литтла
    Size rowIndex(Size row) const;
    Size columnIndex(Size column) const;

    // размерность матрицы
    Size size() const;

    // удаление строки и столбца по индексам
    void removeRowColumn(Size row, Size column);

    Matrix(const Matrix &) = default;
    Matrix &operator=(const Matrix &) = default;

private:
    // инициализация "реальных" номеров строк и столбцов
    void initRowsColumns();

    Vector<Vector<ItemType>> _items;
    // "реальные" номера строк
    Vector<Size> _rows;
    // "реальные" номера столбцов
    Vector<Size> _columns;
};

template<typename ItemType>
inline Matrix<ItemType>::Matrix(Size size, ItemType values)
    :_items(size, Vector<ItemType>(size, values))
    , _rows(size)
    , _columns(size)
{
    initRowsColumns();
}

template<typename ItemType>
inline const ItemType & Matrix<ItemType>::item(Size row, Size column) const
{
    return _items[row][column];
}

template<typename ItemType>
inline ItemType & Matrix<ItemType>::item(Size row, Size column)
{
    // по имя искоренения дублирования кода
    return const_cast<ItemType&>(static_cast<const Matrix<ItemType>&>(*this).item(row, column));
}

template<typename ItemType>
inline const ItemType & Matrix<ItemType>::operator()(Size row, Size column) const
{
    return item(row, column);
}

template<typename ItemType>
inline ItemType & Matrix<ItemType>::operator()(Size row, Size column)
{
    return item(row, column);
}

template<typename ItemType>
inline typename Matrix<ItemType>::Size Matrix<ItemType>::rowIndex(Size row) const
{
    return _rows[row];
}

template<typename ItemType>
inline typename Matrix<ItemType>::Size Matrix<ItemType>::columnIndex(Size column) const
{
    return _columns[column];
}

template<typename ItemType>
inline void Matrix<ItemType>::removeRowColumn(Size row, Size column)
{
    // удаление индексов
    _rows.erase(_rows.begin() + row);
    _columns.erase(_columns.begin() + column);
    // удаление строки
    _items.erase(_items.begin() + row);
    // удаление столбца
    for (Size i = 0; i < _items.size(); i++)
        _items[i].erase(_items[i].begin() + column);
}

template<typename ItemType>
inline typename Matrix<ItemType>::Size Matrix<ItemType>::size() const
{
    return _items.size();
}

template<typename ItemType>
inline void Matrix<ItemType>::initRowsColumns()
{
    for (Size i = 0; i < _rows.size(); i++)
        _rows[i] = _columns[i] = i;
}
