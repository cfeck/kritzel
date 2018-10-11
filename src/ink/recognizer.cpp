#include "recognizer.h"

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QMultiHash>
#include <QTextStream>

#include "libcellwriter.h"

namespace Kritzel
{

class Recognizer::Private
{
public:
    Private() { }
    ~Private() { }

    static Ink parseCellwriterSample(const QString &line, QString &unicode);
    void parseCellwriterProfile();

public:
    ::Ink::Engine engine;
    QMultiHash<QString, Kritzel::Ink> inkDataBase;
};

Ink Recognizer::Private::parseCellwriterSample(const QString &line, QString &unicode)
{
    Ink ink;
    QStringList items = line.simplified().split(QLatin1Char(' '));

    if (items.isEmpty() || items.at(0) != QLatin1String("sample")) {
        unicode.clear();
        return ink;
    }

    unicode = QChar(items.at(1).toInt());
    // TODO int usage = items.at(2).toInt();

    Stroke stroke;
    int pos = 3;

    while (pos < items.size()) {
        if (items.at(pos) == QLatin1Char(';')) {
            ink.append(stroke);
            stroke.clear();
            ++pos;
        } else {
            Sample sample(items.at(pos + 0).toInt(), items.at(pos + 1).toInt());
            stroke.append(sample);
            pos += 2;
        }
    }

    return ink;
}

void Recognizer::Private::parseCellwriterProfile()
{
    QFile profile(QDir::homePath() + QLatin1String("/.cellwriter/profile"));

    if (!profile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return;
    }
    QTextStream text(&profile);
    while (!text.atEnd()) {
        QString line = text.readLine();
        if (line.startsWith(QLatin1String("sample"))) {
            QString unicode;
            Ink ink = parseCellwriterSample(line, unicode);
            if (!unicode.isEmpty() && !ink.isEmpty()) {
                inkDataBase.insert(unicode, ink);
//                qDebug() << "inserted" << unicode << "with" << ink.size() << "strokes";
            }
        }
    }
}

Recognizer::Recognizer(QObject *parent)
    : QObject(parent)
    , d(new Private)
{
    d->parseCellwriterProfile();

    QHashIterator<QString, Kritzel::Ink> it(d->inkDataBase);
    while (it.hasNext()) {
        it.next();
//        qDebug() << "train" << it.key().at(0);
        ::Ink::Glyph glyph(&d->engine);
        foreach (const Stroke &stroke, it.value()) {
            foreach (const Sample &sample, stroke) {
                glyph.addPoint(sample.position().x(), sample.position().y());
            }
            glyph.endStroke();
        }
        glyph.train(it.key().at(0).unicode());
    }
}

Recognizer::~Recognizer()
{
    delete d;
}

QString Recognizer::recognizeInk(const Ink &ink) const
{
    ::Ink::Glyph glyph(&d->engine);
    foreach (const Stroke &stroke, ink) {
        foreach (const Sample &sample, stroke) {
            glyph.addPoint(sample.position().x(), sample.position().y());
        }
        glyph.endStroke();
    }
    int result = glyph.recognize();
    if (result) {
        return QChar(result);
    } else {
        return QString();
    }
}

QList<Ink> Recognizer::trainedSamples(const QString &text) const
{
    return d->inkDataBase.values(text);
}

} //namespace Kritzel

