#include "utils.h"

#include <QDir>
#include <QFile>
#include <QDebug>
#include <QString>
#include <QProcess>
#include <QJsonObject>
#include <QJsonDocument>

#include <QApplication>
#include <QSettings>

#include <QTimer>
#include <QEventLoop>

#include <chrono>
#include <cmath>


ScopedMeasure::ScopedMeasure(const QString& msg, Units units)
    : _start(std::chrono::steady_clock::now())
    , _msg(msg)
    , _units(units)
{
}

ScopedMeasure::~ScopedMeasure()
{
    const auto finish = std::chrono::steady_clock::now();

    if (_units == Units::Milli)
    {
        const std::chrono::duration<double, std::milli> elapsed = finish - _start;
        qd() << _msg << elapsed.count() << "ms";
        return;
    }
    const std::chrono::duration<double, std::micro> elapsed = finish - _start;
    qd() << _msg << elapsed.count() << "us";
}


Measure::Measure(const QString& msg)
    : _start(std::chrono::steady_clock::now())
    , _msg(msg)
{

}

void Measure::start()
{
    _start = std::chrono::steady_clock::now();
}

void Measure::stop()
{
    const auto finish = std::chrono::steady_clock::now();
    const std::chrono::duration<double, std::micro> elapsed = finish - _start;

    qd() << _msg << elapsed.count() << "us";
}

QJsonObject readJsonFile(const QString& fileName)
{
    QFile file(fileName);

    if (!file.open(QFile::ReadOnly))
    {
        qDebug() << "Could not open for read file " << fileName;
        return QJsonObject();
    }

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &error);

    if (error.error != QJsonParseError::NoError)
    {
        qDebug() << "File " << fileName << " validation error (" << error.errorString() << ", offset: " << error.offset << ")";
        return QJsonObject();
    }

    if (doc.isNull())
    {
        qDebug() << "File " << fileName << " is null";
        return QJsonObject();
    }

    if (!doc.isObject())
    {
        qDebug() << "Incorrect format of file " << fileName << ": root must be an object.";
        return QJsonObject();
    }

    return doc.object();
}

QString appDir()
{
    return QApplication::applicationDirPath() + "/";
}

quint16 bitPosition(quint16 n)
{
    return std::log2(n);
}

bool saveDataToFile(const QString& path, const QString& filename, const QByteArray& ba)
{
    // Проверить наличие директории и создать, если ее нет.
    QDir dir(path);
    if (!dir.mkpath("."))
    {
        qd() << "path creation failed: " << path;
        return false;
    }

    QFile file(path + filename);

    if (!file.open(QFile::WriteOnly))
        return false;

    return file.write(ba) > 0;
}

void saveJsonToFile(const QString& path, const QString& filename, const QJsonObject& jo)
{
    saveDataToFile(path, filename, QJsonDocument(jo).toJson(QJsonDocument::Indented));
}

void append(QDataStream& out, const QByteArray& ba)
{
    out.writeRawData(ba.constData(), ba.size());
}

QString doubleToString(double value)
{
    QString num = QString::number(value, 'f', 2);
    if (num.endsWith('0')) num.chop(1);
    if (num.endsWith('0')) num.chop(1);
    if (num.endsWith('.')) num.chop(1);

    return num;
}

QByteArray openAndReadAll(const QString& path)
{
    QFile file(appDir() + path);
    if (file.open(QFile::ReadOnly))
        return file.readAll();
    return QByteArray();
}

QStringList filesInDirectory(QString dir, QStringList nameFilters)
{
    const QString fullDirName = appDir() + dir + "/";
    const QStringList filesInDir = QDir(fullDirName).entryList(nameFilters, QDir::Files | QDir::NoDotAndDotDot);
    return filesInDir;
}

QVariantMap openIniFile(const QString& path)
{
    QSettings sett(appDir() + path, QSettings::IniFormat);

    QStringList allKeys = sett.allKeys();

    QVariantMap values;

    for (const QString& key : allKeys)
        values.insert(key, sett.value(key));

    values.insert("filename", path); // Имя файла сохраняю
    return values;
}

void saveIniFile(const QString& path, const QVariantMap& newData)
{
    QSettings sett(appDir() + path, QSettings::IniFormat);

    for (const QString& key : newData.keys())
        sett.setValue(key, newData.value(key));
}

QString replaceAnsiToRich(QString str)
{
    str = str.replace(lineSeparatorRegExp, "<br>");
    str = str.replace(csi + "0m", "</font>");
    str = str.replace(csi + "32m", "<font color='green'>");
    str = str.replace(csi + "33m", "<font color='orange'>"); // был желтый, но его не видно на белом фоне
    str = str.replace(csi + "34m", "<font color='blue'>");
    str = str.replace(csi + "35m", "<font color='magenta'>");
    str = str.replace(csi + "35m", "<font color='cyan'>");
    return str;
}
