#include "ink.h"

#include <QLineF>
#include <QPainter>

#include <qmath.h>

namespace Kritzel
{

QRectF Stroke::boundingRect(qreal penWidth) const
{
    QRectF rect;

    foreach (const Sample &sample, *this) {
        QRectF sampleRect(sample.position().x() - penWidth * 0.5, sample.position().y() - penWidth * 0.5, penWidth, penWidth);
        rect |= sampleRect;
    }
    return rect;
}

Stroke Stroke::simplified(qreal precision) const
{
    QVector<int> removed;
    int size = this->size();
    removed.reserve(size / 2);

    for (int i = 1; i < size - 1; ++i) {
        if (qAbs(at(i - 1).pressure() -  at(i).pressure()) > 0.05
            || qAbs(at(i + 1).pressure() -  at(i).pressure()) > 0.05)
            continue;

        QLineF line1(at(i - 1).position(), at(i).position());
        QLineF line2(at(i).position(), at(i + 1).position());

        qreal angle = line1.angleTo(line2);
        if (angle > 180) angle = 360 - angle;
        if (angle >= 90)
            continue;

        qreal len = qMax(line1.length(),  line2.length());

        if (qSin(angle * (2 * M_PI / 360)) * len > precision)
            continue;

        removed << i;
        ++i;
    }

    if (removed.isEmpty())
        return *this;

    Stroke stroke;
    stroke.reserve(size - removed.size());

    for (int i = 0, j = 0, r = removed.at(j); i < size; ++i) {
        if (i == r) {
            ++j;
            if (j < removed.size()) {
                r = removed.at(j);
            } else {
                r = 0;
            }
        } else {
            stroke << at(i);
        }
    }

    return stroke;
}

QRectF Ink::boundingRect(qreal penWidth) const
{
    QRectF rect;

    foreach (const Stroke &stroke, *this) {
        rect |= stroke.boundingRect(penWidth);
    }
    return rect;
}

void Ink::draw(QPainter *painter, const QRect &exposedRect) const
{
    Q_UNUSED(exposedRect);

    foreach (const Stroke &stroke, *this) {
        if (stroke.isEmpty()) {
            continue;
        }
        Sample lastSample = stroke.at(0);
        if (stroke.size() == 1) {
            qreal r = painter->pen().widthF() * 0.75;
            painter->drawEllipse(lastSample.position(), r, r);
            continue;
        }
        QVector<QPointF> points;
        points.reserve(stroke.size());
        foreach (const Sample &sample, stroke) {
            lastSample = sample;
            points << sample.position();
        }
        if (points.size()) {
            painter->drawPolyline(points.data(), points.size());
        }
    }
}

} // namespace Kritzel

