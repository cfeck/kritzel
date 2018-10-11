#include "inkitem.h"

#include <QGraphicsView>
#include <QPainter>
#include <QStyleOptionGraphicsItem>

#include <QDebug>

#include "ink.h"

namespace Kritzel
{

class InkItem::Private
{
public:
    Private()
        : stroking(false)
    {
            /* */
    }

    QRectF sampleBoundingRect(const Sample &sample) const;
    qreal samplePenWidth(const Sample &sample) const;

    void drawStroke(QPainter *painter, const Stroke &stroke, const QRectF &exposedRect) const;

public:
    Ink ink;
    QPen pen;

    QRectF boundingRect;
    bool stroking;
};

QRectF InkItem::Private::sampleBoundingRect(const Sample &sample) const
{
    const qreal pad = samplePenWidth(sample) / 2 + 0.1;
    QRectF point = QRectF(sample.position(), QSizeF(0, 0));
    return point.adjusted(-pad, -pad, pad, pad);
}

qreal InkItem::Private::samplePenWidth(const Sample &sample) const
{
    return pen.widthF() * (0.2 + 1.8 * sample.pressure());
}

void InkItem::Private::drawStroke(QPainter *painter, const Stroke &stroke, const QRectF &exposedRect) const
{
    Q_UNUSED(exposedRect);

    if (stroke.isEmpty())
        return;
/*
    foreach (const Sample &sample, stroke) {
        painter->fillRect(sampleBoundingRect(sample).adjusted(-1, -1, 1, 1), Qt::red);
    }
*/

//    painter->fillRect(QRectF(stroke.at(0).position().x(), 0, 5 * stroke.size(), 1), Qt::red);

    if (stroke.size() == 1) {
        QPen pen = this->pen;
        pen.setWidthF(samplePenWidth(stroke.at(0)));
        painter->setPen(pen);
        painter->drawPoint(stroke.at(0).position());
        return;
    }

    Sample lastSample = stroke.at(0);
    qreal lastPressure = lastSample.pressure();

    QVector<QPointF> points;
    points.reserve(stroke.size());

    foreach (const Sample &sample, stroke) {
        if (qAbs(sample.pressure() - lastPressure) > 0.05) {
            if (points.size()) {
                QPen pen = this->pen;
//                QColor color = pen.color();
//                color.setAlphaF(color.alphaF() * (lastSample.pressure() * 0.9 + 0.1));
//                pen.setColor(color);
                pen.setWidthF(samplePenWidth(lastSample));
                pen.setCapStyle(Qt::FlatCap);
                painter->setPen(pen);

                points << sample.position();
                painter->drawPolyline(points.data(), points.size());
                points.clear();
                points << sample.position();
            }
            lastPressure = sample.pressure();
        }
        lastSample = sample;
        points << sample.position();
    }

    if (points.size()) {
        QPen pen = this->pen;
//        QColor color = pen.color();
//        color.setAlphaF(color.alphaF() * (lastSample.pressure() * 0.9 + 0.1));
//        pen.setColor(color);
        pen.setWidthF(samplePenWidth(lastSample));
        pen.setCapStyle(Qt::FlatCap);
        painter->setPen(pen);

        painter->drawPolyline(points.data(), points.size());
    }
}

InkItem::InkItem(QGraphicsItem *parent)
    : QGraphicsItem(parent)
    , d(new Private)
{
    setFlag(QGraphicsItem::ItemUsesExtendedStyleOption, true);
}

InkItem::~InkItem()
{
    delete d;
}

void InkItem::setInk(const Ink &ink)
{
    prepareGeometryChange();
    d->boundingRect = QRectF();
    d->ink = ink;
//     setCacheMode(QGraphicsItem::DeviceCoordinateCache);
}

Ink InkItem::ink() const
{
    return d->ink;
}

void InkItem::setPen(const QPen &pen)
{
    prepareGeometryChange();
    d->boundingRect = QRectF();
    d->pen = pen;
}

QPen InkItem::pen() const
{
    return d->pen;
}

void InkItem::beginStroke()
{
    Q_ASSERT(!d->stroking);

    d->stroking = true;

//     setCacheMode(QGraphicsItem::NoCache);
    prepareGeometryChange();

    d->ink << Stroke();
}

void InkItem::addSample(const Sample &sample)
{
    Q_ASSERT(d->stroking);

    QRectF updateRect = d->sampleBoundingRect(sample);
    d->boundingRect |= updateRect;
    int index = d->ink.size() - 1;
    const Stroke &stroke = d->ink.at(index);

    if (!stroke.isEmpty()) {
        updateRect |= d->sampleBoundingRect(stroke.at(stroke.size() - 1));
    }

    d->ink[index] << sample;

    update(updateRect);
}

void InkItem::endStroke()
{
    Q_ASSERT(d->stroking);
    d->stroking = false;

    int index = d->ink.size() - 1;

    Stroke stroke = d->ink.at(index);
#if 1
    while (true) {
        Stroke simplified = stroke.simplified();
        if (simplified == stroke) break;
        stroke = simplified;
    }
#else
    stroke = stroke.function();
#endif
    d->ink[index] = stroke;

    prepareGeometryChange();
    d->boundingRect = QRectF();
//     setCacheMode(QGraphicsItem::DeviceCoordinateCache);
}

void InkItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    if (!boundingRect().intersects(option->exposedRect))
        return;

    foreach (const Stroke &stroke, d->ink) {
        d->drawStroke(painter, stroke, option->exposedRect);
    }

    if (option->state & QStyle::State_Selected) {
        QPen pen(d->pen.color(), d->pen.widthF(), Qt::DashLine);
        painter->setPen(pen);
        painter->drawRect(boundingRect());
    }
}

QRectF InkItem::boundingRect() const
{
    QRectF bounds;

    if (d->stroking) {
        if (scene()) {
            QList<QGraphicsView *> views = scene()->views();
            foreach(const QGraphicsView *view, views) {
                bounds |= view->mapToScene(view->viewport()->geometry()).boundingRect();
            }
        }
        return bounds;
    }

    if (!d->boundingRect.isEmpty()) {
        return d->boundingRect;
    }

    foreach (const Stroke &stroke, d->ink) {
        foreach (const Sample &sample, stroke) {
            bounds |= d->sampleBoundingRect(sample);
        }
    }

    d->boundingRect = bounds;
    return bounds;
}

} // namespace Kritzel

