#include "placeholder_quick_item.h"

#include <QPen>
#include <QPainter>

#include "utils.h"

//#include "mainwindow.h"
#include "graphics_view.h"
#include <QMainWindow>
#include <QQuickWindow>

#include <QQmlEngine>
#include <QQmlApplicationEngine>

#include <QTimer>

PlaceholderQuickItem::PlaceholderQuickItem(QQuickItem* parent)
    : QQuickPaintedItem(parent)
{
    // setFlag(ItemHasContents);
    // setAcceptHoverEvents(true);
    // setWidth(40);
    // setHeight(40);


    //QWidget* w = QWidget::createWindowContainer(reinterpret_cast<QWindow*>(window()));

    _mw = new GraphicsView();



        qd() << window();


    // QRect screenrect = app.primaryScreen()->geometry();
    // _mw->resize(1920 / 2, screenrect.height());
    // _mw->move(1920/2, screenrect.top());


        QTimer::singleShot(5000, [this]()
        {

            if (_win)
            {
                QWidget* w = QWidget::createWindowContainer(QWindow::fromWinId(_win->winId()));
                _mw->setParent(w);
            }
        });


        //QWidget* w = QWidget::createWindowContainer(QWindow::fromWinId(window->winId()));


    connect(this, &PlaceholderQuickItem::windowChanged, this, [this](QQuickWindow* window)
    {
        _win = window;
//        qd() << this->window();

        // connect(window, &QQuickWindow::heightChanged, this, [this]( ) { qd() << this->window(); } );
        // connect(window, &QQuickWindow::widthChanged, this, [this]( ) { qd() << this->window(); } );
        // connect(window, &QQuickWindow::windowStateChanged, this, [this](auto windowState ) { qd() << this->window(); } );



        return;

        //QWidget* w = QWidget::createWindowContainer(reinterpret_cast<QWindow*>(window));

        //QQmlEngine *engine = QQmlEngine::contextForObject(this)->engine();

        QQmlApplicationEngine* engine = static_cast<QQmlApplicationEngine*>(qmlEngine(this));

        auto wins = engine->rootObjects();
        if (wins.size() > 1)
        {
            QQuickWindow *win=0;
            win=wins.at (0)->findChild<QQuickWindow*>("win1");


            if (win)
            {
                win->setFlags (Qt::FramelessWindowHint);
                //this->setMinimumSize (win->size ());
                //this->resize (win->size ());

                qd() << "win found";
                QWidget* w = QWidget::createWindowContainer(QWindow::fromWinId(win->winId ()));



            }
        }



        //_mw->setParent(w);

        //Qt::WindowFlags flags = _mw->windowFlags();
        //_mw->setWindowFlags(flags | Qt::FramelessWindowHint);

        // _mw->resize(boundingRect().toRect().size());
        // _mw->show();
    });

}

void PlaceholderQuickItem::paint(QPainter* painter)
{
    qd() << "board paint";
    painter->save();
    painter->setPen(QPen(Qt::red, 5, Qt::SolidLine));
    painter->drawRect(QRectF(0, 0, width(), height()));
    painter->restore();
}

void PlaceholderQuickItem::hoverEnterEvent(QHoverEvent* event)
{
    qd() << "hover event enter";
}

void PlaceholderQuickItem::hoverLeaveEvent(QHoverEvent* event)
{
    qd() << "hover event leave";
}

void PlaceholderQuickItem::geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry)
{
    qd() << "geomety changed" << newGeometry;
    // _mw->resize(newGeometry.toRect().size());
    // _mw->move(newGeometry.toRect().topLeft());
    QQuickPaintedItem::geometryChanged(newGeometry, oldGeometry);
}
