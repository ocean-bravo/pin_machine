#include "translate_control.h"
#include "settings.h"
#include "utils.h"

#include <QtQml>
#include <QQmlEngine>
#include <QGuiApplication>
#include <QQmlApplicationEngine>

// Не знаю как получить qmlEngine другим способом, кроме как снаружи.

TranslateControl::TranslateControl(QQmlApplicationEngine* qmlEngine)
    : _qmlEngine(qmlEngine)
{
    _language = Settings::instance().value("common/language", "EN").toString().toUpper();
    changeLanguage(_language);
}

TranslateControl::~TranslateControl()
{

}

void TranslateControl::changeLanguage(const QString& language)
{
    qd() << "changing language to " << language;

    QCoreApplication::removeTranslator(_guiTranslator.data());
    QCoreApplication::removeTranslator(_coreTranslator.data());

    _guiTranslator.reset(new QTranslator);
    _coreTranslator.reset(new QTranslator);

    if (language == "DE")
    {
        _guiTranslator->load(QGuiApplication::applicationDirPath() + "/lang/gui_language_de.qm");
        _coreTranslator->load(QGuiApplication::applicationDirPath() + "/lang/core_language_de.qm");
    }

    if (language == "PL")
    {
        _guiTranslator->load(QGuiApplication::applicationDirPath() + "/lang/gui_language_pl.qm");
        _coreTranslator->load(QGuiApplication::applicationDirPath() + "/lang/core_language_pl.qm");
    }

    if (language == "FR")
    {
        _guiTranslator->load(QGuiApplication::applicationDirPath() + "/lang/gui_language_fr.qm");
        _coreTranslator->load(QGuiApplication::applicationDirPath() + "/lang/core_language_fr.qm");
    }

    QCoreApplication::installTranslator(_guiTranslator.data());
    QCoreApplication::installTranslator(_coreTranslator.data());

    _qmlEngine->retranslate();

    Settings::instance().setValue("common/language", language);
    _language = language;
    emit languageChanged();
}
