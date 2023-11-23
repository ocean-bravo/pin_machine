#include "settings.h"
#include "utils.h"

#include <QSettings>
#include <QApplication>

// Решил не ставить кодек для ini файлов
//_settings->setIniCodec("UTF-8");
// Тогда не возможны не ASCII символы в ini файлах

Settings::Settings()
{
    const QString iniFilePath = QApplication::applicationDirPath() + "/settings.ini";

    _settings = new QSettings(iniFilePath, QSettings::IniFormat, this);
}

void Settings::setValue(const QString& key, const QVariant& value)
{
    _settings->setValue(key, value);
}

QVariant Settings::value(const QString& key, const QVariant& defaultValue) const
{
    return _settings->value(key, defaultValue);
}
