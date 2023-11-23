#pragma once

#include <QThread>
#include <QQmlPropertyMap>

#include <QReadWriteLock>

#include "singleton.h"

#include "utils.h"

class DataBus : public QQmlPropertyMap, public Singleton<DataBus>
{
    Q_OBJECT
    //Q_PROPERTY(double pixelSize READ pixelSize WRITE setPixelSize NOTIFY pixelSizeChanged)

public:
    Q_INVOKABLE double pixelSize() const;
    Q_INVOKABLE void setPixelSize(double);

    Q_INVOKABLE void remove(QString key)
    {
        qd() << "clear " << key;
        clear(key);
    }

    void insert(const QString &key, const QVariant &value);
    QVariant value(const QString& key) const;

signals:
    void pixelSizeChanged();

protected:
//    template <typename Derived>
//    explicit DataBus(Derived* derived, QObject* parent = nullptr)
//        : QQmlPropertyMap(derived, parent)
//    {}

private:
    DataBus(QObject * parent  = nullptr);
    ~DataBus();

    mutable QReadWriteLock _lock;

    friend class Singleton<DataBus>;


};

inline DataBus& db()
{
    return DataBus::instance();
}
