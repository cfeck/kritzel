#ifndef KRITZEL_WINDOW_H
#define KRITZEL_WINDOW_H 1

#include <QMainWindow>

namespace Kritzel
{

class Window : public QMainWindow
{
    Q_OBJECT

public:
    explicit Window(QWidget *parent = nullptr);
    ~Window();

public Q_SLOTS:
    void clear();
    void zoomIn();
    void zoomOut();

private:
    class Private;
    Private * const d;
};

} // namespace Kritzel

#endif // KRITZEL_WINDOW_H
