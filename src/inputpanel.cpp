#include "inputpanel.h"

#include <QAction>
#include <QMouseEvent>
#include <QPainter>

#include <QDebug>

#include "cell.h"
#include "recognizer.h"

namespace Kritzel
{

class InputPanel::Private
{
public:
    Recognizer recognizer;
    InputPanel *q;
    QSize size;
};

InputPanel::InputPanel(QWidget *parent)
    : QWidget(parent)
    , d(new Private)
{
    d->q = this;
#if 0
    setContextMenuPolicy(Qt::ActionsContextMenu);
    QAction *testAction = new QAction(QStringLiteral("Clear"), this);
    addAction(testAction);
#endif

    int x = 4;
    for (int p = 0; p < 20; ++p) {
        const int y = 4;
        Cell *cell = new Cell(&d->recognizer, this);
        d->size = cell->sizeHint();
        cell->resize(d->size);
        cell->move(x, y);
        x += cell->width() + 2;
    }
}

InputPanel::~InputPanel()
{
    delete d;
}

QSize InputPanel::sizeHint() const
{
    return QSize(20 * d->size.width() + 19 * 2, d->size.height()) + QSize(8, 8);
}

void InputPanel::paintEvent(QPaintEvent *)
{
    QPainter p(this);

    p.fillRect(rect(), QColor(220, 220, 200));
}

} // namespace Kritzel

