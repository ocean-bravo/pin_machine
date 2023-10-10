#pragma once

#include <QString>
#include <QByteArray>
#include <QDataStream>
#include <QDebug>

#include <chrono>
#include <tuple>
#include <exception>

class QJsonObject;

const QString csi("\033[");
const QString up("1A");
const QString fwd10("10C");


inline QString toReal(double value)
{
    return QString::number(value, 'f', 3);
}

class ScopedMeasure
{
public:
    enum Units { Micro, Milli };
    ScopedMeasure(const QString& msg, Units = Units::Micro);
    ~ScopedMeasure();

private:
    std::chrono::steady_clock::time_point _start;
    QString _msg;
    Units _units = Units::Micro;
};

class Measure
{
public:
    Measure(const QString& msg);

    void start();
    void stop();

private:
    std::chrono::steady_clock::time_point _start;
    QString _msg;
};

QJsonObject readJsonFile(const QString& fileName);
bool saveDataToFile(const QString& path, const QString& filename, const QByteArray& ba);
void saveJsonToFile(const QString& path, const QString& filename, const QJsonObject& jo);
QString appDir();

// Позиция самого старшего бита
quint16 bitPosition(quint16 n);

inline auto qd()
{
    return qDebug().noquote();
}

// Специализация для типа QByteArray
// Если QByteArray просто << в QDataStream, спереди запишется еще размер массива, что не нужно.
void append(QDataStream& out, const QByteArray& ba);

template<typename T, std::size_t N>
void append(QDataStream& out, const std::array<T, N>& ba)
{
    for (size_t i = 0; i < N; ++i)
        append(out, ba[i]);
}

template<typename T>
void append(QDataStream& out, const T& value)
{
    out << value;
}

template<typename... Args>
QByteArray prepare(Args... args)
{
    QByteArray data;
    QDataStream ds(&data, QIODevice::WriteOnly);
    ds.setFloatingPointPrecision(QDataStream::SinglePrecision);
    ((append(ds, args)), ...);
    return data;
}

template <typename T>
class asKeyValueRange
{
public:
    asKeyValueRange(const T& data) : _data{data} { }
    auto begin() const { return _data.keyValueBegin(); }
    auto end() const { return _data.keyValueEnd(); }
private:
    const T& _data;
};

// С удалением 2-х нулей после точки и самой точки
QString doubleToString(double value);

void printXmlToDebug(const QByteArray& data);

// value в диапазоне [min(a,b), max(a,b)]
template<typename T>
bool inRange(T value, T a, T b)
{
    return value >= std::min(a, b) && value <= std::max(a, b);
}
