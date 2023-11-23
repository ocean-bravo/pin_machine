#pragma once

#include "singleton.h"

#include <QObject>
#include <QVariant>

class QString;
class QSettings;

class Settings : public QObject, public Singleton<Settings>
{
    Q_OBJECT

public:
    Q_INVOKABLE void setValue(const QString& key, const QVariant& value);
    Q_INVOKABLE QVariant value(const QString& key, const QVariant& defaultValue = QVariant()) const;

private:
    Settings();

    QSettings* _settings = nullptr;

    friend class Singleton<Settings>;
};

inline Settings& settings()
{
    return Settings::instance();
}
