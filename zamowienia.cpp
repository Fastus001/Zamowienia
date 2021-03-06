#include "zamowienia.h"
#include "ui_zamowienia.h"
#include <QtWidgets>



Zamowienia::Zamowienia(QWidget *parent) :
    QMainWindow(parent, Qt::FramelessWindowHint),
    ui(new Ui::Zamowienia)
{
    ui->setupUi(this);
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

void Zamowienia::on_makeOrderButton_clicked()
{
    zamGenerator zGen;
    zGen.setModal(false);
    zGen.exec();
}

void Zamowienia::on_import_to_sql_pushButton_clicked()
{
    import_do_sql importSql;
    importSql.setModal(false);
    importSql.exec();
}
