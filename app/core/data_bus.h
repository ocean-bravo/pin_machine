#pragma once

#include <QThread>
#include <QQmlPropertyMap>

#include <QReadWriteLock>

#include "singleton.h"

#include "utils.h"

class DataBusRef;

class DataBus : public QQmlPropertyMap, public Singleton<DataBus>
{
    Q_OBJECT
    //Q_PROPERTY(double pixelSize READ pixelSize WRITE setPixelSize NOTIFY pixelSizeChanged)
    Q_PROPERTY(QString messageBox READ messageBox WRITE setMessageBox NOTIFY messageBoxChanged)
    Q_PROPERTY(QString messageBoxResult READ messageBoxResult WRITE setMessageBoxResult NOTIFY messageBoxResultChanged)


public:
    Q_INVOKABLE double pixInMm() const;
    Q_INVOKABLE void setPixInMm(double);

    double widthToPixInMm(int width);

    Q_INVOKABLE void remove(QString key)
    {
        qd() << "clear " << key;
        clear(key);
    }

    void insert(const QString &key, const QVariant &value);
    QVariant value(const QString& key) const;

    DataBusRef operator[](const QString& key);

signals:
    void pixelSizeChanged();
    void imageChanged(QString key);
    void messageBoxChanged();
    void messageBoxResultChanged();

protected:
//    template <typename Derived>
//    explicit DataBus(Derived* derived, QObject* parent = nullptr)
//        : QQmlPropertyMap(derived, parent)
//    {}
    //QVariant updateValue(const QString &key, const QVariant &input) override;

private:
    DataBus(QObject * parent  = nullptr);
    ~DataBus();

    QString messageBox() const;
    void setMessageBox(QString msg);

    QString messageBoxResult() const;
    void setMessageBoxResult(QString msg);

    mutable QReadWriteLock _lock;

    friend class Singleton<DataBus>;
};

class DataBusRef
{
public:
    void operator = (const QVariant& value)
    {
        DataBus::instance().insert(_key, value);
    }

private:
    DataBusRef(const QString& key)
    {
        _key = key;
    }
    QString _key;

    friend class DataBus;
};

inline DataBusRef db(const QString& key)
{
    return DataBus::instance()[key];
}

inline DataBus& db()
{
    return DataBus::instance();
}
