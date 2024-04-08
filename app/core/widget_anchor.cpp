#include "widget_anchor.h"

#include <QQuickItem>
#include <QWidget>

//#include "utils.h"

//WidgetAnchor::WidgetAnchor(QObject* parent, QWidget* widget, QQuickItem* quickitem)
WidgetAnchor::WidgetAnchor(QWidget* widget, QQuickItem* quickitem)
    : QObject(widget)
    , _widget(widget)
    , _quickItem(quickitem)
{
    connect(_quickItem, &QQuickItem::xChanged,      this, &WidgetAnchor::updateGeometry);
    connect(_quickItem, &QQuickItem::yChanged,      this, &WidgetAnchor::updateGeometry);
    connect(_quickItem, &QQuickItem::widthChanged,  this, &WidgetAnchor::updateGeometry);
    connect(_quickItem, &QQuickItem::heightChanged, this, &WidgetAnchor::updateGeometry);

    updateGeometry();
}

void WidgetAnchor::updateGeometry()
{
    //qd() << "update geometry";

    if (_quickItem)
    {
        // 1. Не канает
        //QRectF r = _quickItem->mapRectToItem(_quickItem, QRectF(_quickItem->x(), _quickItem->y(), _quickItem->width(), _quickItem->height()));

        // 2. лучше
        //QRectF r = _quickItem->mapRectToScene(QRectF(_quickItem->x(), _quickItem->y(), _quickItem->width(), _quickItem->height()));

        // 3. Отлично!
        QRectF r = _quickItem->mapRectToScene(QRectF(0, 0, _quickItem->width(), _quickItem->height()));

        _widget->setGeometry(r.toRect());


        // 1. Работает, не как хочется,поэтому костыли
        // auto pos = _quickItem->position().toPoint();
        // pos.rx() += 845 ;
        // pos.ry() += 25;
        // _widget->move(pos);
    }
}
