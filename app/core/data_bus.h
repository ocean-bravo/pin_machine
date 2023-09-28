#pragma once

#include <QThread>
#include <QQmlPropertyMap>

#include "singleton.h"

class DataBus : public QQmlPropertyMap, public Singleton<DataBus>
{
    Q_OBJECT

public:

    Q_INVOKABLE void remove(QString key)
    {
        clear(key);
    }

public slots:


signals:


protected:
    template <typename Derived>
    explicit DataBus(Derived* derived, QObject* parent = nullptr)
        : QQmlPropertyMap(derived, parent)
    {}

private:

    DataBus(QObject * parent  = nullptr);
    ~DataBus();

    friend class Singleton<DataBus>;
};

inline DataBus& db()
{
    return DataBus::instance();
}
