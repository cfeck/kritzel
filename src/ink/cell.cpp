#include "cell.h"

#include <QApplication>
#include <QMouseEvent>
#include <QPainter>

#include <QDebug>
#include <QTimer>

#include "recognizer.h"

namespace Kritzel
{

class Cell::Private
{
public:
    Private()
        : cellRect(0, 0, 40, 70)
        , stroking(false)
        , cursorVisible(true)
        , inkVisible(true)
        , error(false)
    {
        /* */
    }
    ~Private() { }

    QPointF mouseToEngine();

    void beginStroke();
    void updateStroke();
    void endStroke();

    void recognize();
    void trackMouse(QMouseEvent *event) {
#if 0
        qDebug() << "trackMouse"
            << "pos" << event->pos()
            << "localPos" << event->localPos();
#endif
        mouse = event->localPos();
    }
    void trackMouse(QTabletEvent *event) {
#if 0
        qDebug() << "trackTablet"
            << "pos" << event->pos()
            << "hires" << event->hiResGlobalPos()
            << "globlal" << event->globalPos();
#endif
        mouse = event->pos();
    }

public:
    Cell *q;

    QRect cellRect;

    QPointF mouse;
    bool stroking : 1;
    bool tabletStroke : 1;

    bool cursorVisible : 1;
    bool inkVisible : 1;
    bool textVisible : 1;
    bool error : 1;

    Ink ink;
    Recognizer *recognizer;
    QString text;
};

QPointF Cell::Private::mouseToEngine()
{
    QPointF p = mouse - cellRect.center();
    p *= 256.0 / cellRect.height();
    if (p.x() < -128) p.setX(-128);
    if (p.y() < -128) p.setY(-128);
    if (p.x() > 128) p.setX(128);
    if (p.y() > 128) p.setY(128);
//    qDebug() << "Coords" << "mouse" << mouse << "center" << cellRect.center() << "point" << p;
    return p;
}

void Cell::Private::beginStroke()
{
    if (!text.isEmpty() || error) {
        text.clear();
        ink.clear();
    }
    inkVisible = true;
    textVisible = false;
    error = false;
    if (!stroking) {
        /* new stroke */
        ink << Stroke();
    }
    stroking = true;
    ink[ink.size() - 1] << Sample(mouseToEngine(), 1.0);
    q->update();
}

void Cell::Private::updateStroke()
{
    if (stroking) {
        ink[ink.size() - 1] << Sample(mouseToEngine(), 1.0);
    }
    q->update();
}

void Cell::Private::recognize()
{
    if (inkVisible && text.isEmpty()) {
        QString result = recognizer->recognizeInk(ink);
        text = result;
        if (!result.isEmpty()) {
            ink = recognizer->trainedSamples(result).at(0);
            textVisible = true;
            inkVisible = false;
            q->update();
        } else {
            if (!ink.isEmpty()) {
                error = true;
                q->update();
            }
        }
    }
}

void Cell::Private::endStroke()
{
    stroking = false;
}

Cell::Cell(Recognizer *recognizer, QWidget *parent)
    : QWidget(parent)
    , d(new Private)
{
    d->q = this;
    d->recognizer = recognizer;
    setMouseTracking(true);
//    setCursor(Qt::BlankCursor);
//    setCursor(Qt::CrossCursor);
#if 0
    QPixmap cursorPixmap(3, 3);
    QPainter p(&cursorPixmap);
    p.fillRect(cursorPixmap.rect(), Qt::transparent);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.setPen(Qt::NoPen);
    p.setBrush(Qt::NoBrush);
    p.drawEllipse(QRect(0, 0, 3, 3));
    setCursor(cursorPixmap);
#endif
    qApp->installEventFilter(this);
}

Cell::~Cell()
{
    qApp->removeEventFilter(this);
    delete d;
}

QSize Cell::sizeHint() const
{
    return d->cellRect.size();
}

void Cell::enterEvent(QEvent *)
{
    update();
}

void Cell::leaveEvent(QEvent *)
{
    d->recognize();
    update();
}

void Cell::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        d->tabletStroke = false;
        d->trackMouse(event);
        d->beginStroke();
        event->accept();
    }
}

void Cell::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        d->tabletStroke = false;
        d->trackMouse(event);
        d->beginStroke();
        event->accept();
    }
}

void Cell::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && !d->tabletStroke) {
        d->endStroke();
        event->accept();
    }
}

void Cell::mouseMoveEvent(QMouseEvent *event)
{
    if (!d->tabletStroke) {
        d->trackMouse(event);
        d->updateStroke();
        event->accept();
    }
    update();
}

bool Cell::eventFilter(QObject *watched, QEvent *event)
{
    if (!watched->isWidgetType()) {
        switch (event->type()) {
        case QEvent::TabletEnterProximity:
        case QEvent::TabletLeaveProximity:
            tabletEvent((QTabletEvent *) event);
        default:
            break;
        }
    }
    return QObject::eventFilter(watched, event);
}

void Cell::tabletEvent(QTabletEvent *event)
{
#if 0
    switch (event->type()) {
    case QEvent::TabletPress:
        if (rect().contains(event->pos())) {
/*            qDebug() << "TabletPress"
                << "type" << event->pointerType()
                << "id" << event->uniqueId()
                << "device" << event->device()
                << "mod" << event->modifiers();
*/            d->tabletStroke = true;
            d->trackMouse(event);
            d->beginStroke();
            event->accept();
            return;
        }
        break;
    case QEvent::TabletRelease:
        if (d->tabletStroke) {
            d->endStroke();
            event->accept();
            return;
        }
        break;
    case QEvent::TabletMove:
/*        qDebug() << "TabletMove";
*/        d->trackMouse(event);
        if (d->tabletStroke) {
            d->updateStroke();
        }
        update();
        event->accept();
        return;
    case QEvent::TabletEnterProximity:
        d->cursorVisible = true;
        update();
        break;
    case QEvent::TabletLeaveProximity:
        d->recognize();
        d->cursorVisible = false;
        update();
        break;
    }
#endif
    QWidget::tabletEvent(event);
}

void Cell::paintEvent(QPaintEvent *)
{
    QPainter p(this);

    p.fillRect(rect(), Qt::white);
#if 0
    QColor cellBackground(200, 200, 200);
    QColor cellUnderline(180, 180, 180);
    if (d->error) {
        cellBackground = QColor(220, 180, 180);
        cellUnderline = QColor(200, 160, 160);
    } else if (!d->text.isEmpty()) {
        cellBackground = QColor(240, 240, 240);
        cellUnderline = QColor(220, 220, 220);
    }
#else
    QColor cellBackground(200, 200, 180);
    QColor cellUnderline(180, 180, 160);
    if (d->error) {
        cellBackground = QColor(220, 180, 180);
        cellUnderline = QColor(200, 160, 160);
    } else if (!d->text.isEmpty()) {
        cellBackground = QColor(220, 220, 200);
        cellUnderline = QColor(210, 210, 190);
    }
#endif

    p.fillRect(d->cellRect, cellBackground);
    p.fillRect(d->cellRect.adjusted(0, 2 * d->cellRect.height() / 3, 0, 0), cellUnderline);

    qreal factor = d->cellRect.height() / 256.0;
    p.setClipRect(d->cellRect);
    p.setRenderHint(QPainter::Antialiasing, true);

    if (d->textVisible) {
        QFont font(QStringLiteral("Monospace"));
        font.setStyleHint(QFont::Monospace);
        font.setFixedPitch(true);
//        font.setBold(true);
        font.setPixelSize(d->cellRect.height() * 0.75);
        p.setFont(font);
        p.setPen(Qt::darkBlue);
        p.drawText(d->cellRect.adjusted(0, -6, 0, -6), Qt::AlignCenter, d->text);
    }

    if (d->cursorVisible && underMouse() && false) {
        p.setPen(Qt::NoPen);
        p.setBrush(Qt::black);
        p.drawEllipse(d->mouse, 1.5, 1.5);
    }

    if (d->inkVisible) {
        p.translate(d->cellRect.center());
        p.scale(factor, factor);
        p.setPen(QPen(d->error ? Qt::red : Qt::darkBlue, 1.5 / factor));
        p.setBrush(Qt::NoBrush);
        d->ink.draw(&p);
    }
}

} // namespace Kritzel

