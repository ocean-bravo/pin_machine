#include "my_image_provider_plugin.h"
#include "my_image_provider.h"

#include <qqml.h>

void QmlMyImageProviderPlugin::registerTypes(const char* uri)
{
    Q_ASSERT(uri == QLatin1String("MyImageProvider"));
    qmlRegisterType<MyImageProvider>(uri, 1, 0, "MyImageProvider");
}

void QmlMyImageProviderPlugin::initializeEngine(QQmlEngine* engine, const char* uri)
{
    Q_UNUSED(uri);
    engine->addImageProvider("myImages", new MyImageProvider);
}
