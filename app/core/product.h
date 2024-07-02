#pragma once

#include "singleton.h"
#include "utils.h"

#include <QQmlPropertyMap>

class Product : public QQmlPropertyMap, public Singleton<Product>
{
    Q_OBJECT
    // //Q_PROPERTY(double pixelSize READ pixelSize WRITE setPixelSize NOTIFY pixelSizeChanged)
    // Q_PROPERTY(QString messageBox READ messageBox WRITE setMessageBox NOTIFY messageBoxChanged)
    // Q_PROPERTY(QString messageBoxResult READ messageBoxResult WRITE setMessageBoxResult NOTIFY messageBoxResultChanged)


public:
    Q_INVOKABLE void save(const QString &url);
    Q_INVOKABLE void load(const QString &url);

signals:

protected:


private:
    Product(QObject* parent  = nullptr);
    ~Product();


    friend class Singleton<Product>;
};

inline Product& product()
{
    return Product::instance();
}
