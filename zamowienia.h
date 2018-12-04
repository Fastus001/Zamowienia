#ifndef ZAMOWIENIA_H
#define ZAMOWIENIA_H

#include <QMainWindow>

namespace Ui {
class Zamowienia;
}

class Zamowienia : public QMainWindow
{
    Q_OBJECT

public:
    explicit Zamowienia(QWidget *parent = nullptr);
    ~Zamowienia();

private:
    Ui::Zamowienia *ui;
};

#endif // ZAMOWIENIA_H
