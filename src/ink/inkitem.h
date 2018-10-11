#ifndef KRITZEL_INKITEM_H
#define KRITZEL_INKITEM_H 1

#include <QGraphicsItem>

namespace Kritzel
{

class Ink;
class Sample;

class InkItem : public QGraphicsItem
{
public:
    InkItem(QGraphicsItem *parent = nullptr);
    ~InkItem();

public:
    void setInk(const Ink &ink);
    Ink ink() const;
    void setPen(const QPen &pen);
    QPen pen() const;

    void beginStroke();
    void addSample(const Sample &sample);
    void endStroke();

public:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    QRectF boundingRect() const override;

private:
    class Private;
    Private * const d;
};

} // namespace Kritzel

#endif // KRITZEL_INKITEM_H
