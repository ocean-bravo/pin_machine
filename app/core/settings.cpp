#include "settings.h"
#include "utils.h"

// Решил не ставить кодек для ini файлов
//_settings->setIniCodec("UTF-8");
// Тогда не возможны не ASCII символы в ini файлах

Settings::Settings(QString filename)
    : QSettings(filename, QSettings::IniFormat)
{ }


void Settings::setValue(const QString& key, const QVariant& value)
{
    QSettings::setValue(key, value);
}

QVariant Settings::value(const QString& key, const QVariant& defaultValue) const
{
    return QSettings::value(key, defaultValue);
}
