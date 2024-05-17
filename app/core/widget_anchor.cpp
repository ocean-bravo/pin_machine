#include "widget_anchor.h"

#include <QQuickItem>
#include <QWidget>

#include "utils.h"

//WidgetAnchor::WidgetAnchor(QObject* parent, QWidget* widget, QQuickItem* quickitem)
WidgetAnchor::WidgetAnchor(QWidget* widget, QQuickItem* quickitem1, QQuickItem* quickitem2)
    : QObject(widget)
    , _widget(widget)
    , _quickItem1(quickitem1)
    , _quickItem2(quickitem2)
{
    connect(_quickItem1, &QQuickItem::xChanged,       this, &WidgetAnchor::updateGeometry);
    connect(_quickItem1, &QQuickItem::yChanged,       this, &WidgetAnchor::updateGeometry);
    connect(_quickItem1, &QQuickItem::widthChanged,   this, &WidgetAnchor::updateGeometry);
    connect(_quickItem1, &QQuickItem::heightChanged,  this, &WidgetAnchor::updateGeometry);
    connect(_quickItem1, &QQuickItem::visibleChanged, this, &WidgetAnchor::updateGeometry);

    connect(_quickItem2, &QQuickItem::xChanged,       this, &WidgetAnchor::updateGeometry);
    connect(_quickItem2, &QQuickItem::yChanged,       this, &WidgetAnchor::updateGeometry);
    connect(_quickItem2, &QQuickItem::widthChanged,   this, &WidgetAnchor::updateGeometry);
    connect(_quickItem2, &QQuickItem::heightChanged,  this, &WidgetAnchor::updateGeometry);
    connect(_quickItem2, &QQuickItem::visibleChanged, this, &WidgetAnchor::updateGeometry);

    updateGeometry();
}

void WidgetAnchor::updateGeometry()
{
    _widget->setVisible(_quickItem1->isVisible() || _quickItem2->isVisible());

    if (_widget->isVisible())
    {
        if (_quickItem1->isVisible())
        {
            updateGeometryItem(_quickItem1);
            return;
        }

        if (_quickItem2->isVisible())
        {
            updateGeometryItem(_quickItem2);
            return;
        }
    }
}

void WidgetAnchor::updateGeometryItem(QQuickItem* item)
{
    // 1. Не канает
    //QRectF r = _quickItem->mapRectToItem(_quickItem, QRectF(_quickItem->x(), _quickItem->y(), _quickItem->width(), _quickItem->height()));

    // 2. лучше
    //QRectF r = _quickItem->mapRectToScene(QRectF(_quickItem->x(), _quickItem->y(), _quickItem->width(), _quickItem->height()));

    // 3. Отлично!
    //QRectF r = _quickItem1->mapRectToScene(QRectF(0, 0, _quickItem1->width(), _quickItem1->height()));

    QPointF p1 = item->mapToScene(QPointF(0, 0)); //, _quickItem1->width(), _quickItem1->height()));
    QPointF p2 = item->mapToScene(QPointF(item->width(), item->height()));

    //qd() << p1 << p2;

    _widget->setGeometry(QRect(p1.toPoint(), p2.toPoint()));

    // 1. Работает, не как хочется,поэтому костыли
    // auto pos = _quickItem->position().toPoint();
    // pos.rx() += 845 ;
    // pos.ry() += 25;
    // _widget->move(pos);
}
