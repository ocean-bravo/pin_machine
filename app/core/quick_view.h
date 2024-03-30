#pragma once

#include <QQuickWidget>

class QuickScene;

class QuickView : public QQuickWidget
{
    Q_OBJECT
    //Q_PROPERTY(KDSME::StateMachineScene *scene READ scene WRITE setScene NOTIFY sceneChanged)


public:
    explicit QuickView(QWidget *parent = nullptr);
    ~QuickView();


    Q_INVOKABLE void fitInView();


    QuickScene* scene() const;
    QRectF adjustedViewRect();


    QQuickItem* sceneObject() const;
protected:

private:

    QuickScene* _scene;

};
