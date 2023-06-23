#include "my_image_provider.h"

MyImageProvider::MyImageProvider()
    : QObject()
    , QQuickImageProvider(QQuickImageProvider::Image)
{
}
