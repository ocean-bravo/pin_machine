#include "product.h"
#include "settings.h"
#include "scene.h"

#include <QSplashScreen>
#include <QScopeGuard>
#include <QFile>
#include <QUrl>
#include <QBuffer>


Product::Product(QObject *parent)
    : QQmlPropertyMap(this, parent)
{
    insert("z_cut", double(0.0));
    insert("z_set", double(0.0));
    insert("z_park", double(0.0));
    insert("w_podpor", double(0.0));
    insert("w_park", double(0.0));

    insert("pin_name", QString("00-00000"));
    insert("product_name", QString("Не задано"));
}

Product::~Product()
{

}

void Product::save(const QString& url)
{
    QSplashScreen splash(QPixmap("./splash.png"), Qt::WindowType(Qt::SplashScreen + Qt::FramelessWindowHint + Qt::WindowStaysOnTopHint));
    splash.setEnabled(false);
    splash.setWindowModality(Qt::ApplicationModal);
    splash.show();

    int count = scene().images();

    auto connection = connect(&scene(), &Scene::imageSaved, this, [&splash, count](int i)
    {
        splash.showMessage(QString("Saved %1 of %2 images").arg(i).arg(count));
    });

    auto guard = qScopeGuard([=]() { disconnect(connection); });


    QByteArray sceneBa = scene().saveSceneToByteArray();
    QVariantMap productInfo;
    productInfo.insert("product_name", value("product_name"));
    productInfo.insert("pin_name", value("pin_name"));
    productInfo.insert("z_cut",    value("z_cut"));
    productInfo.insert("z_set",    value("z_set"));
    productInfo.insert("z_park",   value("z_park"));
    productInfo.insert("w_podpor", value("w_podpor"));
    productInfo.insert("w_park",   value("w_park"));

    QByteArray ba;
    QDataStream out(&ba, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_15);
    out << sceneBa << productInfo;

    Measure mes3("safetofile");

    QFile file(QUrl(url).toLocalFile());

    if (!file.open(QFile::WriteOnly))
    {
        qd() << "couldnt open file for save: " << QUrl(url).toLocalFile();
        return;
    }

    file.write(ba);

    mes3.stop();
}

void Product::load(const QString& url)
{
    QSplashScreen splash(QPixmap("./splash.png"), Qt::WindowType(Qt::SplashScreen + Qt::FramelessWindowHint + Qt::WindowStaysOnTopHint));
    splash.setEnabled(false);
    splash.setWindowModality(Qt::ApplicationModal);
    //splash.show();

    QFile file(QUrl(url).toLocalFile());

    if (!file.exists())
    {
        qd() << "file not exists: " << url << QUrl(url).toLocalFile();
        return;
    }

    if (!file.open(QFile::ReadOnly))
    {
        qd() << "cant opent file: " << url << QUrl(url).toLocalFile();
        return;
    }

    const QByteArray ba = file.readAll();
    file.close();

    QByteArray sceneBa;
    QVariantMap productInfo;
    QDataStream in(ba);
    in.setVersion(QDataStream::Qt_5_15);
    in >> sceneBa >> productInfo;

    insert("product_name",    productInfo.value("product_name"));
    insert("pin_name", productInfo.value("pin_name"));
    insert("z_cut",    productInfo.value("z_cut"));
    insert("z_set",    productInfo.value("z_set"));
    insert("z_park",   productInfo.value("z_park"));
    insert("w_podpor", productInfo.value("w_podpor"));
    insert("w_park",   productInfo.value("w_park"));

    scene().loadSceneFromByteArray(sceneBa);
}
