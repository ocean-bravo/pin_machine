#include "widget_anchor.h"

#include "utils.h"

WidgetAnchor::WidgetAnchor(QObject* parent, QWidget* widget, QQuickItem* item)
    : QObject(parent)
    , _widget(widget)
    , _quickItem(item)
{
    connect(_quickItem, &QQuickItem::xChanged,      this, &WidgetAnchor::updateGeometry);
    connect(_quickItem, &QQuickItem::yChanged,      this, &WidgetAnchor::updateGeometry);
    connect(_quickItem, &QQuickItem::widthChanged,  this, &WidgetAnchor::updateGeometry);
    connect(_quickItem, &QQuickItem::heightChanged, this, &WidgetAnchor::updateGeometry);

    updateGeometry();
}

void WidgetAnchor::updateGeometry()
{
    qd() << "update geometry";


    if (_quickItem)
    {
        QRectF r = _quickItem->mapRectToItem(_quickItem, QRectF(_quickItem->x(), _quickItem->y(), _quickItem->width(), _quickItem->height()));
        _widget->setGeometry(r.toRect());
    }
}
