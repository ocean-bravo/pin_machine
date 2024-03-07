#pragma once

#include "singleton.h"

#include <QObject>
#include <QVariant>
#include <QSettings>

class QString;
class QSettings;

class Settings : public QSettings, public Singleton<Settings>
{
    Q_OBJECT

public:
    Q_INVOKABLE void setValue(const QString& key, const QVariant& value);
    Q_INVOKABLE QVariant value(const QString& key, const QVariant& defaultValue = QVariant()) const;

private:
    Settings();

    friend class Singleton<Settings>;
};

inline Settings& settings()
{
    return Settings::instance();
}
