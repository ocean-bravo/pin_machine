#pragma once

#include <QQmlExtensionPlugin>

class Q_DECL_EXPORT QmlMyImageProviderPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QQmlExtensionInterface_iid)

public:
    void registerTypes(const char* uri) override;
    void initializeEngine(QQmlEngine* engine, const char* uri) override;
};
