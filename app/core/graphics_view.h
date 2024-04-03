#pragma once

#include <QRubberBand>
#include <QGraphicsView>
#include <QScopedPointer>
#include <QMouseEvent>

#include <QPainterPath>

class QRubberBand;

class GraphicsView : public QGraphicsView
{
    Q_OBJECT

    //void zoom(double factor);
public:
    GraphicsView(QWidget* parent = nullptr);
    ~GraphicsView();

    // Заполнить платой вид
    void fit();

    void setSizeToFit(QSizeF size);

signals:
    void sizeChanged(QSize);
    void selectModeChanged(bool mode);
    void scanPosition(QPointF pos, QString scene);
    void addBlob(QPointF pos);
    void calcPath(QPointF pos);
    void setSelectedAsPunch(bool state);
    void deleteSelectedBlobs();

protected:
    void wheelEvent(QWheelEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;


    void contextMenuEvent(QContextMenuEvent* event) override;


    void resizeEvent(QResizeEvent *event) override;
    bool event(QEvent* event) override;

    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void focusOutEvent(QFocusEvent *event) override;


private:
    QSizeF _sizeToFit;
    QScopedPointer<QRubberBand> _rb;
    QPoint _origin;


    bool _dragMode = false;
    QMouseEvent _lastMouseEvent = {QEvent::None, QPointF(), QPointF(), QPointF(), Qt::NoButton, { }, { } };
    //bool _selectMode = false;
    QPainterPath _selArea;

    friend class QmlGraphicsView;
};
