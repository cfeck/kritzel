#include <QApplication>

#include "window.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    Kritzel::Window window;
    window.show();
    return app.exec();
}

