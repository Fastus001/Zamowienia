#ifndef ZAMOWIENIA_H
#define ZAMOWIENIA_H

#include <QMainWindow>
#include <QCursor>

namespace Ui {
class Zamowienia;
}

class Zamowienia : public QMainWindow
{
    Q_OBJECT

public:
    explicit Zamowienia(QWidget *parent = nullptr);
    ~Zamowienia() override;

protected:
    void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;

private:
    Ui::Zamowienia *ui;
    QPoint dragPosition;
};

#endif // ZAMOWIENIA_H
