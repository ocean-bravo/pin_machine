#pragma once

#include <QObject>
#include <QQuickItem>
#include <QPointer>
#include <QWidget>

class WidgetAnchor: public QObject
{
    Q_OBJECT

public:
    WidgetAnchor(QObject* parent, QWidget* widget, QQuickItem* item);
    //WidgetAnchor(QWidget* widget, QQuickItem* item);
private:
    void updateGeometry();

    QWidget* _widget = nullptr;
    QQuickItem* _quickItem = nullptr;
};
