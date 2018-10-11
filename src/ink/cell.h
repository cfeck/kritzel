#ifndef KRITZEL_CELL_H
#define KRITZEL_CELL_H 1

#include <QWidget>

#include "ink.h"

namespace Kritzel
{

class Recognizer;

class Cell : public QWidget
{
    Q_OBJECT

public:
    explicit Cell(Recognizer *recognizer, QWidget *parent = nullptr);
    ~Cell();

public:
    void setInk(const Ink &ink);
    Ink ink() const;
    void setText(const QString &text);
    QString text() const;
    void setInkVisible(bool visible);
    bool isInkVisible() const;
    void setTextVisible(bool visible);
    bool isTextVisible() const;

Q_SIGNALS:
    Kritzel::Ink inkChanged();
    QString textChanged();
    void cellDeleted();

public:
    QSize sizeHint() const override;

protected:
    void enterEvent(QEvent *event) override;
    void leaveEvent(QEvent *event) override;

    void mousePressEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void tabletEvent(QTabletEvent *event) override;

    void paintEvent(QPaintEvent *pe) override;

private:
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    class Private;
    Private * const d;
};

} // namespace Kritzel

#endif // KRITZEL_CELL_H
