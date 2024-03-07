#include "settings.h"
#include "utils.h"

#include <QSettings>
#include <QApplication>

// Решил не ставить кодек для ini файлов
//_settings->setIniCodec("UTF-8");
// Тогда не возможны не ASCII символы в ini файлах

namespace {
const QString iniFilePath = QApplication::applicationDirPath() + "/settings.ini";
}

Settings::Settings()
    : QSettings(iniFilePath, QSettings::IniFormat)
{ }


void Settings::setValue(const QString& key, const QVariant& value)
{
    QSettings::setValue(key, value);
}

QVariant Settings::value(const QString& key, const QVariant& defaultValue) const
{
    return QSettings::value(key, defaultValue);
}
