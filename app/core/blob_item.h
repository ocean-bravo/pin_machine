#pragma once

#include <QGraphicsEllipseItem>

class BlobItem : public QObject, public QGraphicsEllipseItem
{
    Q_OBJECT

public:
    BlobItem(double x, double y, double dia, QGraphicsItem* parent = nullptr);

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0) override;

    bool isFiducial() const;
    void setFiducial(bool state);

    bool isPunch() const;
    void setPunch(bool state);

    bool isRealFiducial() const;
    void setRealFiducial(bool state);

    void setHighlight(bool state);

    void setSceneFileName(QString sceneFile);
    QString sceneFileName() const;

signals:
    void pressed();

protected:
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

    void contextMenuEvent(QGraphicsSceneContextMenuEvent* event);

    //QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

    QRectF boundingRect() const override;
    QPainterPath shape() const override;

private:
    void highlight();
    void unhighlight();
    void repaintLater();

    double _highlightedThickness = 3.0;
    double _nonhighlightedThickness = 1.0;
    bool _highlighted = false;
    QString _sceneFileName;
};
