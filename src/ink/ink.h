#ifndef KRITZEL_INK_H
#define KRITZEL_INK_H 1

#include <QVector>
#include <QRectF>

#include "sample.h"

class QPainter;

namespace Kritzel
{

class Stroke : public QVector<Sample>
{
public:
    QRectF boundingRect(qreal penWidth = 1.0) const;

    Stroke simplified(qreal precision = 0.5) const;
};

class Ink : public QVector<Stroke>
{
public:
    QRectF boundingRect(qreal penWidth = 1.0) const;

    void draw(QPainter *painter, const QRect &exposedRect = QRect()) const;
};

} // namespace Kritzel

#endif // KRITZEL_INK_H
