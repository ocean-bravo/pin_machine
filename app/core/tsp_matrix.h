#pragma once

#include <QVector>

template<typename ItemType>
class Matrix
{
public:
    explicit Matrix(int size = 0, ItemType values = 0);

    // доступ к элементам по индексам
    const ItemType &item(int row, int column) const;
    ItemType &item(int row, int column);
    const ItemType &operator()(int row, int column) const;
    ItemType &operator()(int row, int column);

    // получение "реальных" идексов
    // e.g. если была удалена строка 1 rowIndex(1) == 2
    // ?необходимо для алгоритма Литтла
    int rowIndex(int row) const;
    int columnIndex(int column) const;

    // размерность матрицы
    int size() const;

    // удаление строки и столбца по индексам
    void removeRowColumn(int row, int column);

    Matrix(const Matrix &) = default;
    Matrix &operator=(const Matrix &) = default;

private:
    // инициализация "реальных" номеров строк и столбцов
    void initRowsColumns();

    QVector<QVector<ItemType>> _items;
    // "реальные" номера строк
    QVector<int> _rows;
    // "реальные" номера столбцов
    QVector<int> _columns;
};

template<typename ItemType>
inline Matrix<ItemType>::Matrix(int size, ItemType values)
    :_items(size, QVector<ItemType>(size, values))
    , _rows(size)
    , _columns(size)
{
    initRowsColumns();
}

template<typename ItemType>
inline const ItemType & Matrix<ItemType>::item(int row, int column) const
{
    return _items[row][column];
}

template<typename ItemType>
inline ItemType & Matrix<ItemType>::item(int row, int column)
{
    // по имя искоренения дублирования кода
    return const_cast<ItemType&>(static_cast<const Matrix<ItemType>&>(*this).item(row, column));
}

template<typename ItemType>
inline const ItemType & Matrix<ItemType>::operator()(int row, int column) const
{
    return item(row, column);
}

template<typename ItemType>
inline ItemType & Matrix<ItemType>::operator()(int row, int column)
{
    return item(row, column);
}

template<typename ItemType>
inline int Matrix<ItemType>::rowIndex(int row) const
{
    return _rows[row];
}

template<typename ItemType>
inline int Matrix<ItemType>::columnIndex(int column) const
{
    return _columns[column];
}

template<typename ItemType>
inline void Matrix<ItemType>::removeRowColumn(int row, int column)
{
    // удаление индексов
    _rows.erase(_rows.begin() + row);
    _columns.erase(_columns.begin() + column);
    // удаление строки
    _items.erase(_items.begin() + row);
    // удаление столбца
    for (int i = 0; i < _items.size(); i++)
        _items[i].erase(_items[i].begin() + column);
}

template<typename ItemType>
inline int Matrix<ItemType>::size() const
{
    return _items.size();
}

template<typename ItemType>
inline void Matrix<ItemType>::initRowsColumns()
{
    for (int i = 0; i < _rows.size(); i++)
        _rows[i] = _columns[i] = i;
}
