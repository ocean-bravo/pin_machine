#include "my_image_provider.h"

#include <QtGlobal>

MyImageProvider::MyImageProvider()
    : QObject()
    , QQuickImageProvider(QQuickImageProvider::Image)
{
}

void MyImageProvider::clearCaptured()
{
    _lock.lockForWrite();

    QStringList keysToRemove;
    for (const QString& k : _images.keys())
    {
        if (k.contains("captured_"))
            keysToRemove.append(k);
    }

    for (const QString& k : qAsConst(keysToRemove))
    {
        _images.remove(k);
    }

    _lock.unlock();
}
