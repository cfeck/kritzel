#ifndef KRITZEL_INPUTPANEL_H
#define KRITZEL_INPUTPANEL_H 1

#include <QWidget>

namespace Kritzel
{

class InputPanel : public QWidget
{
    Q_OBJECT

public:
    explicit InputPanel(QWidget *parent = nullptr);
    ~InputPanel();

public:
    QSize sizeHint() const override;

public:
    void paintEvent(QPaintEvent *pe) override;

private:
    class Private;
    Private * const d;
};

} // namespace Kritzel

#endif // KRITZEL_INPUTPANEL_H
