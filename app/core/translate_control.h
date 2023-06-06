#pragma once

#include <QObject>
#include <QString>
#include <QTranslator>
#include <QScopedPointer>

class QQmlApplicationEngine;

class TranslateControl : public QObject
{
    Q_OBJECT
    Q_PROPERTY (QString language MEMBER _language NOTIFY languageChanged)
    Q_PROPERTY (QStringList availableLanguages MEMBER _availableLanguages CONSTANT)

public:
    TranslateControl(QQmlApplicationEngine* qmlEngine);
    ~TranslateControl();

    Q_INVOKABLE void changeLanguage(const QString& language);

signals:
    void languageChanged();

private:
    QQmlApplicationEngine* _qmlEngine;
    QString _language;
    QStringList _availableLanguages = {"DE", "EN", "FR", "PL"};
    QScopedPointer<QTranslator> _guiTranslator;
    QScopedPointer<QTranslator> _coreTranslator;
};
