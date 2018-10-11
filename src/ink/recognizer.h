#ifndef KRITZEL_RECOGNIZER_H
#define KRITZEL_RECOGNIZER_H 1

#include <QObject>

#include "ink.h"

namespace Kritzel
{

class Recognizer : public QObject
{
    Q_OBJECT

public:
    Recognizer(QObject *parent = nullptr);
    ~Recognizer();

public:
    QString recognizeInk(const Ink &ink) const;
    QList<Ink> trainedSamples(const QString &text) const;

private:
    class Private;
    Private * const d;
};

} // namespace Kritzel

#endif // KRITZEL_RECOGNIZER_H
