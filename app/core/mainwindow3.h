#pragma once

#include <QMainWindow>

class QResizeEvent;

namespace Ui { class MainWindow3; }

class MainWindow3 : public QMainWindow
{
    Q_OBJECT

    Q_PROPERTY(int width READ width NOTIFY sizeChanged FINAL)
    Q_PROPERTY(int height READ height NOTIFY sizeChanged FINAL)

public:
    MainWindow3(QWidget* parent = nullptr);
    ~MainWindow3();

signals:
    void sizeChanged();

protected:
    void resizeEvent(QResizeEvent* event);

private:
    Ui::MainWindow3* ui;
};
