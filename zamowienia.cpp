#include "zamowienia.h"
#include "ui_zamowienia.h"
#include <QtWidgets>


Zamowienia::Zamowienia(QWidget *parent) :
    QMainWindow(parent, Qt::FramelessWindowHint),
    ui(new Ui::Zamowienia)
{
    ui->setupUi(this);
    ui->statusBar->showMessage("Baza danych nie jest podłączona!!");
}

Zamowienia::~Zamowienia()
{
    delete ui;
}

void Zamowienia::mouseMoveEvent(QMouseEvent *event)
{
    //event->buttons() & Qt::LeftButton
    if (event->buttons() & Qt::LeftButton) {
        move(event->globalPos() - dragPosition);
        event->accept();
    }
}

void Zamowienia::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        dragPosition = event->globalPos() - frameGeometry().topLeft();
        event->accept();
    }
}

void Zamowienia::on_connectToSql_clicked()
{
    //podłaczenie bazy danych
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("localhost");
    db.setUserName("root");
    db.setPassword("");
    db.setDatabaseName("zabawki");

    if(db.open())
        ui->statusBar->showMessage("Baza danych podłączona");
    else
        ui->statusBar->showMessage("Nie udało się podłączyć bazy danych");

}
