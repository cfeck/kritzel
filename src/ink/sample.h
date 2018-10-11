#ifndef KRITZEL_SAMPLE_H
#define KRITZEL_SAMPLE_H 1

#include <QPointF>

namespace Kritzel
{

class Sample
{
public:
    Sample();
    Sample(const QPointF &position, qreal pressure = 1.0);
    Sample(qreal positionX, qreal positionY, qreal pressure = 1.0);
    ~Sample();

public:
    QPointF position() const;
    qreal pressure() const;

    bool operator==(const Sample &that) const;

private:
    QPointF m_position;
    qreal m_pressure;
};

inline Sample::Sample()
    : m_position(QPointF())
    , m_pressure(0.0)
{
    /* */
}

inline Sample::Sample(const QPointF &position, qreal pressure)
    : m_position(position)
    , m_pressure(pressure)
{
    /* */
}

inline Sample::Sample(qreal positionX, qreal positionY, qreal pressure)
    : m_position(QPointF(positionX, positionY))
    , m_pressure(pressure)
{
    /* */
}

inline Sample::~Sample()
{
    /* */
}

inline QPointF Sample::position() const
{
    return m_position;
}

inline qreal Sample::pressure() const
{
    return m_pressure;
}

inline bool Sample::operator==(const Sample &that) const
{
    return m_position == that.m_position && m_pressure == that.m_pressure;
}

} // namespace Kritzel

#endif // KRITZEL_SAMPLE_H
