#pragma once

#include <QPoint>
#include <QMap>
#include <QHash>
#include <QGraphicsItem>
#include <QLocale>

#include <utility>

class QGraphicsScene;
class QGraphicsRectItem;
class QGraphicsItem;
class QSizeF;

template<typename T>
bool is(QGraphicsItem* item)
{
    return dynamic_cast<T*>(item);
}

template<typename T>
bool is(const QGraphicsItem* item)
{
    return dynamic_cast<const T*>(item);
}

template<typename T>
bool isNot(QGraphicsItem* item)
{
    return !bool(dynamic_cast<T*>(item));
}


template<typename T>
using Action = std::function<void(T*)>;

template<typename T>
void everyChild(const QGraphicsItem* parent, const Action<T>& action)
{
    const QList<QGraphicsItem*> childItems = parent->childItems();

    for (QGraphicsItem* item : childItems)
    {
        T* t = dynamic_cast<T*>(item);

        if (!t)
            continue;

        action(t);
    }
}

template<typename T, typename ItemType>
void every(const QList<ItemType*>& items, const Action<T>& action)
{
    for (ItemType* item : items)
    {
        T* t = dynamic_cast<T*>(item);

        if (!t)
            continue;

        action(t);
    }
}

// template<typename T>
// void every(const QList<QGraphicsItem*>& items, const Action<T>& action)
// {
//     for (QGraphicsItem* item : items)
//     {
//         T* t = dynamic_cast<T*>(item);

//         if (!t)
//             continue;

//         action(t);
//     }
// }

template<typename Parent, typename Child>
bool atLeastOne(const QGraphicsItem* parent)
{
    const QList<QGraphicsItem*> childItems = parent->childItems();

    for (QGraphicsItem* item : childItems)
    {
        if (isNot<Parent>(item))
            continue;

        if (isNot<Child>(item))
            return true;
    }
    return false;
}

template<typename T>
QList<T*> convertTo(const QList<QGraphicsItem*>& items)
{
    QList<T*> otherItems;
    every<T>(items, [&otherItems](T* t) { otherItems.append(t); });
    return otherItems;
}


// Десятичный разделитель - точка
//inline QString toReal(double value)
//{
//    return QString::number(value, 'f', 2);
//}

inline QString toInt(int value)
{
    return QString::number(value);
}

// Десятичный разделитель - в зависимости от системной локали
//inline QString toRealLocal(double value)
//{
//    return QLocale::system().toString(value, 'f', 2);
//}

inline double fromRealLocal(const QString& value)
{
    return QLocale::system().toDouble(value);
}
