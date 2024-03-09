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
const QString beginprevline(csi + "F");
inline const QString setpos(int column) { return csi + QString("%1C").arg(column); }


inline QString toReal3(double value)
{
    return QString::number(value, 'f', 3);
}

inline QString toReal2(double value)
{
    return QString::number(value, 'f', 2);
}

inline QString toReal1(double value)
{
    return QString::number(value, 'f', 1);
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


#include <linux/videodev2.h>

inline QString fourccToString(quint32 fourcc)
{
    QString str;
    for(uint32_t i=0; i<4; i++)
    {
        str += (char)(fourcc & 0xFF);
        fourcc >>= 8;
    }
    return str;
}

inline quint32 fourccToInt(QString fourcc)
{
    if (fourcc.size() != 4)
    {
        qd() << "error: wrong FOURCC size: " << fourcc;
        return 0;
    }

    return v4l2_fourcc(fourcc[0].toLatin1(), fourcc[1].toLatin1(), fourcc[2].toLatin1(), fourcc[3].toLatin1());
}

inline QByteArray v4l2_fourcc2s(quint32 fourcc)
{
    QByteArray buf(8, '\0');
    buf[0] = fourcc & 0x7f;
    buf[1] = (fourcc >> 8) & 0x7f;
    buf[2] = (fourcc >> 16) & 0x7f;
    buf[3] = (fourcc >> 24) & 0x7f;
    if (fourcc & (1 << 31))
    {
        buf[4] = '-';
        buf[5] = 'B';
        buf[6] = 'E';
    }

    return buf;
}

QByteArray openAndReadAll(const QString& path);

QVariantMap openIniFile(const QString& path);
void saveIniFile(const QString& path, const QVariantMap& newData);
