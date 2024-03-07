#pragma once

#include "singleton.h"

#include <QObject>
#include <QVariant>
#include <QSettings>
#include <QApplication>


class QString;
class QSettings;

class Settings : public QSettings, public Singleton<Settings>
{
    Q_OBJECT

public:
    Q_INVOKABLE void setValue(const QString& key, const QVariant& value);
    Q_INVOKABLE QVariant value(const QString& key, const QVariant& defaultValue = QVariant()) const;

private:
    Settings(QString filename = QApplication::applicationDirPath() + "/settings.ini");

    friend class Singleton<Settings>;
};

inline Settings& settings()
{
    return Settings::instance();
}
