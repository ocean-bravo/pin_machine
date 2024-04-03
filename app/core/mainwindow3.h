#pragma once

#include <QMainWindow>

class QGraphicsScene;

namespace Ui { class MainWindow3; }

class MainWindow3 : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow3(QWidget* parent = nullptr);
    ~MainWindow3();

private:
    Ui::MainWindow3* ui;
};
