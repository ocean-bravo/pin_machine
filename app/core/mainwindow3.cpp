#include "mainwindow3.h"
#include "ui_mainwindow3.h"

MainWindow3::MainWindow3(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow3)
{
    ui->setupUi(this);

    //Supports only the background, background-clip and background-origin properties.

    //setWindowFlags(Qt::Window | Qt::FramelessWindowHint);

    // QString style = R"(

    //     QMainWindow::separator {
    //         background: yellow;
    //         width: 10px; /* when vertical */
    //         height: 10px; /* when horizontal */
    //     }


    //     QDockWidget {
    //         titlebar-close-icon: url(theme:Dark/close.svg);
    //         titlebar-normal-icon: url(theme:Dark/popout.svg);
    //     }

    //     QDockWidget::title {
    //         text-align: center;
    //         background-color: rgb(70,69,70);
    //     }

    //     QDockWidget::close-button, QDockWidget::float-button {
    //         border: 1px solid transparent;
    //         background: transparent;
    //         padding: 0px;
    //     }

    //     QDockWidget::close-button:hover, QDockWidget::float-button:hover {
    //         background: transparent;
    //     }

    //     QDockWidget::close-button:pressed, QDockWidget::float-button:pressed {
    //         padding: 1px -1px -1px 1px;
    //     }
    //     )";


    // QMainWindow {
    //     background-color: rgb(80, 90, 10);
    // }

    // QWidget#MainWindow3 {
    //     background-color: red;
    // }



    // QMainWindow::separator:hover {
    //     background: red;
    // }

    // QMainWindow::centralWidget {
    //     background: yellow;
    //     width: 10px;
    //     height: 10px;
    // }

    // QMainWindow::centralWidget:hover {
    //     background: red;
    // }

    // QFrame {
    //     border: 2px solid green;
    //     border-radius: 4px;
    //     padding: 2px;
    //     background-color: red;
    //     color: blue;
    // }


    //setStyleSheet(style);
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
