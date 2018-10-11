#include "window.h"

#include <QMenuBar>
#include <QVBoxLayout>

#include "inputpanel.h"
#include "view.h"

namespace Kritzel
{

class Window::Private
{
public:
    Window *q;

    View *view;
    InputPanel *inputPanel;
};

Window::Window(QWidget *parent)
    : QMainWindow(parent)
    , d(new Private)
{
    d->q = this;

    QAction *quitAction = menuBar()->addAction(QStringLiteral("Quit"));
    connect(quitAction, SIGNAL(triggered()), this, SLOT(close()));
#if 0
    QAction *clearAction = menuBar()->addAction(QStringLiteral("Clear"));
    connect(clearAction, SIGNAL(triggered()), this, SLOT(clear()));

    QAction *zoomInAction = menuBar()->addAction(QStringLiteral("Zoom +"));
    connect(zoomInAction, SIGNAL(triggered()), this, SLOT(zoomIn()));

    QAction *zoomOutAction = menuBar()->addAction(QStringLiteral("Zoom -"));
    connect(zoomOutAction, SIGNAL(triggered()), this, SLOT(zoomOut()));
#endif
    QWidget *central = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(central);
    layout->setMargin(0);

//    d->view = new View(this);
    d->inputPanel = new InputPanel(this);
//    d->inputPanel->setFixedHeight(d->inputPanel->sizeHint().height());
    central->setFixedSize(d->inputPanel->sizeHint());
    layout->addWidget(d->inputPanel);
//    layout->addWidget(d->view);

    setCentralWidget(central);
    setFixedSize(sizeHint());
}

Window::~Window()
{
    delete d;
}

void Window::clear()
{
    d->view->scene()->clear();
}

void Window::zoomIn()
{
    d->view->scale(2, 2);
}

void Window::zoomOut()
{
    d->view->scale(0.5, 0.5);
}

} // namespace Kritzel

