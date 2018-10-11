#include "view.h"

#include <QMouseEvent>
#include <QTabletEvent>

#include <QDebug>

#include "inkitem.h"
#include "ink.h"

namespace Kritzel
{

class View::Private
{
public:
    Private()
        : currentInkItem(nullptr)
    {
        QImage pattern(16, 16, QImage::Format_ARGB32_Premultiplied);
        QPainter p(&pattern);
        for (int i = 0; i < 4; ++i) {
            const int v = i == 1 || i == 2 ? 176 : 196;
            p.fillRect(QRect((i & 1) * 8, (i & 2) * 4, 8, 8), QColor(v, v, v));
        }
        p.end();
        backgroundBrush = QBrush(QPixmap::fromImage(pattern));
    }

public:
    View *q;

    InkItem *currentInkItem;
    QPen currentPen;

    bool tabletStroke;

    void beginStroke(const Sample &sample);
    void updateStroke(const Sample &sample);
    void endStroke(const Sample &sample);

    Sample mouseSample(const QMouseEvent *event) const;
    Sample tabletSample(const QTabletEvent *event) const;

    QBrush backgroundBrush;
    QBrush gridBrush;
};

Sample View::Private::mouseSample(const QMouseEvent *event) const
{
    return q->mapToScene(event->pos());
}

Sample View::Private::tabletSample(const QTabletEvent *event) const
{
    QPointF position = event->posF();
    QPainterPath path;
    path.moveTo(position);
    path = q->mapToScene(path);
    position = path.elementAt(0);
    return Sample(position, event->pressure());
}

void View::Private::beginStroke(const Sample &sample)
{
    Q_ASSERT(!currentInkItem);
    currentInkItem = new InkItem;
    q->scene()->addItem(currentInkItem);
    currentInkItem->setPen(currentPen);
    currentInkItem->beginStroke();
    currentInkItem->addSample(sample);
}

void View::Private::updateStroke(const Sample &sample)
{
    Q_ASSERT(currentInkItem);
    currentInkItem->addSample(sample);
}

void View::Private::endStroke(const Sample &sample)
{
    Q_UNUSED(sample);
    Q_ASSERT(currentInkItem);
//    currentInkItem->addSample(sample);
    currentInkItem->endStroke();
    currentInkItem = nullptr;
}

View::View(QWidget *parent)
    : QGraphicsView(parent)
    , d(new Private)
{
    d->q = this;

    setRenderHint(QPainter::Antialiasing, true);

    QPen pen;
    pen.setColor(palette().color(QPalette::Text));
//    pen.setColor(QColor(100, 100, 100, 200));
//    pen.setColor(QColor(255, 255, 255, 00));
    pen.setWidthF(1.0); 
    pen.setCapStyle(Qt::RoundCap);
    pen.setJoinStyle(Qt::RoundJoin);
    d->currentPen = pen;

    QGraphicsScene *scene = new QGraphicsScene(this);
    setScene(scene);
    setAlignment(Qt::AlignTop | Qt::AlignLeft);
}

View::~View()
{
    delete d;
}

QSize View::sizeHint() const
{
    int fontHeight = fontMetrics().height();
    return QSize(30 * fontHeight, 40 * fontHeight);
}

void View::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && !d->currentInkItem) {
        d->tabletStroke = false;
        d->beginStroke(d->mouseSample(event));
    }
}

void View::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && !d->currentInkItem) {
        d->tabletStroke = false;
        d->beginStroke(d->mouseSample(event));
    }
}

void View::mouseMoveEvent(QMouseEvent *event)
{
    if (d->currentInkItem && !d->tabletStroke)
        d->updateStroke(d->mouseSample(event));
}

void View::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && d->currentInkItem && !d->tabletStroke)
        d->endStroke(d->mouseSample(event));
}

void View::tabletEvent(QTabletEvent *event)
{
    switch (event->type()) {
    case QEvent::TabletPress:
        if (viewport()->geometry().contains(event->pos()) && !d->currentInkItem) {
            d->tabletStroke = true;
            d->beginStroke(d->tabletSample(event));
            event->accept();
            return;
        }
        break;
    case QEvent::TabletRelease:
        if (d->currentInkItem && d->tabletStroke) {
            d->endStroke(d->tabletSample(event));
            event->accept();
            return;
        }
        break;
    case QEvent::TabletMove:
        if (d->currentInkItem && d->tabletStroke) {
            d->updateStroke(d->tabletSample(event));
            event->accept();
            return;
        }
    default:
        break;
    }
    QGraphicsView::tabletEvent(event);
}

void View::drawBackground(QPainter *painter, const QRectF &rect)
{
return;
    painter->setTransform(QTransform());
    painter->setRenderHint(QPainter::SmoothPixmapTransform, false);
    painter->fillRect(rect, d->backgroundBrush);
}

void View::drawForeground(QPainter *painter, const QRectF &rect)
{
return;
    QImage pattern(16, 16, QImage::Format_ARGB32_Premultiplied);
    pattern.fill(0);
    QPainter p(&pattern);
    p.setPen(QPen(Qt::black, 0.5, Qt::DotLine));
    p.drawLine(0, 0, 15, 0);
    p.drawLine(0, 0, 0, 15);
    p.end();
    painter->setTransform(QTransform());
    painter->setRenderHint(QPainter::SmoothPixmapTransform, false);
    painter->fillRect(rect, QBrush(QPixmap::fromImage(pattern)));
}

} // namespace Kritzel

