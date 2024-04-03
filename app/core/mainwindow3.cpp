#include "mainwindow3.h"
#include "ui_mainwindow3.h"

#include "wait.h"
#include "utils2.h"
#include "common.h"
#include "data_bus.h"



#include <QMessageBox>
#include <QGraphicsScene>
#include <QGraphicsEllipseItem>
#include <QLabel>
#include <QJsonObject>
#include <QShortcut>

#include <functional>


namespace {



}

MainWindow3::MainWindow3(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow3)
{
    ui->setupUi(this);


}

MainWindow3::~MainWindow3()
{
    delete ui;
}
