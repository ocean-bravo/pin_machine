#pragma once

#include <QMainWindow>

class QGraphicsScene;

namespace Ui { class MainWindow; }

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private:
    void updateCameraView();
    Ui::MainWindow* ui;
    QGraphicsScene*  _scene;
};
