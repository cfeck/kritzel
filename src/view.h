#ifndef KRITZEL_VIEW_H
#define KRITZEL_VIEW_H 1

#include <QGraphicsView>

namespace Kritzel
{

class View : public QGraphicsView
{
    Q_OBJECT

public:
    explicit View(QWidget *parent = nullptr);
    ~View();

public:
    QSize sizeHint() const override;

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void tabletEvent(QTabletEvent *event) override;

    void drawBackground(QPainter *painter, const QRectF &rect) override;
    void drawForeground(QPainter *painter, const QRectF &rect) override;

private:
    class Private;
    Private * const d;
};

} // namespace Kritzel

#endif // KRITZEL_VIEW_H
