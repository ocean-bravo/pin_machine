#include "process_plugin.h"
#include "process.h"

#include <qqml.h>

void QmlProcessPlugin::registerTypes(const char* uri)
{
    Q_ASSERT(uri == QLatin1String("Process"));
    qmlRegisterType<Process>(uri, 1, 0, "Process");
}
