#pragma once

#include <QObject>


class QWidget;
class QQuickItem;

class WidgetAnchor: public QObject
{

public:
    WidgetAnchor(QObject* parent, QWidget* widget, QQuickItem* quickitem);

private:
    void updateGeometry();

    QWidget* _widget = nullptr;
    QQuickItem* _quickItem = nullptr;
};
