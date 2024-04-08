#include "mainwindow3.h"
#include "ui_mainwindow3.h"

MainWindow3::MainWindow3(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow3)
{
    ui->setupUi(this);

QString style = R"(

    QMainWindow {
        background-color: rgb(80, 90, 10);
    }

    centralWidget {
    background: yellow;
    width: 10px; /* when vertical /
    height: 10px; / when horizontal */
    }

    centralWidget:hover {
    background: red;
    }
    )";



    setStyleSheet(style);
}

MainWindow3::~MainWindow3()
{
    delete ui;
}

void MainWindow3::resizeEvent(QResizeEvent* event)
{
    QMainWindow::resizeEvent(event);
    emit sizeChanged();
}
