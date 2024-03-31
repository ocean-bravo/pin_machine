#include "quick_view.h"
#include "ui_mainwindow.h"

#include "wait.h"
#include "utils2.h"
#include "common.h"
#include "data_bus.h"


#include "quick_scene.h"


#include <QMessageBox>
#include <QGraphicsScene>
#include <QGraphicsEllipseItem>
#include <QLabel>
#include <QJsonObject>
#include <QShortcut>
#include <QQmlEngine>
#include <QQmlContext>

#include <functional>



QuickView::QuickView(QWidget *parent)
    : QQuickWidget(parent)
{

    engine()->rootContext()->setContextProperty(QStringLiteral("_quickView"), this);

}

QuickView::~QuickView()
{

}

QQuickItem* QuickView::sceneObject() const
{
    QQuickItem *item = rootObject()->findChild<QQuickItem *>(QStringLiteral("stateMachineScene"));
    Q_ASSERT(item);
    return item;
}


void QuickView::fitInView()
{
    QRectF sceneRect = scene()->boundingRect();

    QRectF viewRect = adjustedViewRect();

    if (sceneRect.isEmpty() || viewRect.isEmpty())
        return;

    qreal horizontalScale = viewRect.width() / sceneRect.width();
    qreal verticalScale = viewRect.height() / sceneRect.height();
    const qreal uniformScale = qMin(horizontalScale, verticalScale);
    scene()->zoomBy(uniformScale);
}

QRectF QuickView::adjustedViewRect()
{
    static const int margin = 10;

    const QQuickItem *viewPort = rootObject()->findChild<QQuickItem *>(QStringLiteral("scrollView"));

    const QRectF viewRect(viewPort->x(), viewPort->y(), viewPort->width(), viewPort->height());
    if (viewRect.isEmpty())
        return QRectF();

    return viewRect.adjusted(margin, margin, -margin, -margin);
}

QuickScene* QuickView::scene() const
{
    return _scene;
}

