#pragma once

#include <QRubberBand>
#include <QGraphicsView>
#include <QScopedPointer>

class QRubberBand;

class GraphicsView : public QGraphicsView
{
    Q_OBJECT

    //void zoom(double factor);
public:
    GraphicsView(QWidget* parent = nullptr);

    // Заполнить платой вид
    void fit();

    void setSizeToFit(QSizeF size);

signals:
    void sizeChanged(QSize);
    void selectModeChanged(bool mode);

protected:
    void wheelEvent(QWheelEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void resizeEvent(QResizeEvent *event) override;
    bool event(QEvent* event) override;

private:
    QSizeF _sizeToFit;
    QScopedPointer<QRubberBand> _rb;
    QPoint _origin;
};
