#pragma once

#include <QObject>

class QWidget;
class QQuickItem;

class WidgetAnchor: public QObject
{
public:
    //WidgetAnchor(QObject* parent, QWidget* widget, QQuickItem* quickitem1);
    WidgetAnchor(QWidget* widget, QQuickItem* quickitem1, QQuickItem* quickitem2);

private:
    void updateGeometry();
    void updateGeometryItem(QQuickItem* item);

    QWidget* _widget = nullptr;
    QQuickItem* _quickItem1 = nullptr;
    QQuickItem* _quickItem2 = nullptr;
};
