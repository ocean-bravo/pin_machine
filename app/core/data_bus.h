#pragma once

#include <QThread>
#include <QQmlPropertyMap>

#include "singleton.h"

class DataBus : public QQmlPropertyMap, public Singleton<DataBus>
{
    Q_OBJECT

public:


public slots:


signals:


protected:


private:

    DataBus();
    ~DataBus();

    friend class Singleton<DataBus>;
};

inline DataBus& db()
{
    return DataBus::instance();
}
