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
    /*
    QString kod = "748415";
    QSqlQuery query("SELECT * FROM `towary` WHERE kod = "+kod);


    int i = query.size();
    QString tekst;
    tekst.setNum(i);
    QMessageBox::information(this,"Udało się",tekst);
    */
}

void Zamowienia::on_exelFilePathButton_clicked()
{
    excelFilePath = QFileDialog::getOpenFileName(this,"Wybierz arkusz Excel z listą towarów do zamówienia, "
                                                      "pamiętaj aby nie było polskich znaków po drodze!!!!","C:/","*.xls *.xlsx");
}

void Zamowienia::on_searchItemsInSqlButton_clicked()
{
    if(excelFilePath.isEmpty())
    {
        QMessageBox::information(this, "Brak pliku excel","Nie został wybrany arkusz excel z listą produktów do zamówienia!!");
    }else{

        using namespace libxl;
        Book* book;
        if(excelFilePath.contains(".xlsx"))
        {
            book = xlCreateXMLBook();
        }else{
            book = xlCreateBook();
        }
        QByteArray ba = excelFilePath.toUtf8();

        std::string konwersja = excelFilePath.toStdString();


        if(book->load(ba.data()))
        {
            Sheet *sheet = book->getSheet(0);
            if(sheet)
            {
                int row = 16;
                int col = 1;
                QString kod;
                std::string kodString;
                CellType cellType = sheet->cellType(row, col);
                while (cellType==CELLTYPE_NUMBER | cellType==CELLTYPE_STRING)
                {

                    switch (cellType)
                      {
                                           case CELLTYPE_EMPTY: break;
                                           case CELLTYPE_NUMBER:
                                           {
                                               double d = sheet->readNum(row, col);
                                               kod.setNum(d,'g',13);
                                               QMessageBox::information(this,"Kod z arkusza excel",kod);
                                               break;
                                           }
                                           case CELLTYPE_STRING:
                                           {
                                               kodString = sheet->readStr(row, col);
                                               QString A = kodString.c_str();
                                               QMessageBox::information(this,"Kod z arkusza excel",A);
                                               break;
                                           }
                                           case CELLTYPE_BOOLEAN: break;
                                           case CELLTYPE_BLANK:break;
                                           case CELLTYPE_ERROR:break;
                      }

                   row++;
                   cellType = sheet->cellType(row, col);
                }

            }

        }
        book->release();

    }
}
