#include "zamowienia.h"
#include "ui_zamowienia.h"

Zamowienia::Zamowienia(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Zamowienia)
{
    ui->setupUi(this);
}

Zamowienia::~Zamowienia()
{
    delete ui;
}
