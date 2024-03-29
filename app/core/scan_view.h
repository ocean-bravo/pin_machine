#pragma once

#include <QMainWindow>

class QGraphicsScene;

namespace Ui { class ScanView; }

class ScanView : public QMainWindow
{
    Q_OBJECT

public:
    ScanView(QWidget* parent = nullptr);
    ~ScanView();

private:
    void updateCameraView();
    Ui::ScanView* ui;
    QGraphicsScene*  _scene;
};
