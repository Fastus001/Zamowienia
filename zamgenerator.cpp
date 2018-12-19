#include "zamgenerator.h"
#include "ui_zamgenerator.h"

zamGenerator::zamGenerator(QWidget *parent) :
    QDialog(parent, Qt::FramelessWindowHint),
    ui(new Ui::zamGenerator)
{
    ui->setupUi(this);
    ui->setExceFileToOrder->setVisible(false);
    openSqlDataBase();
    openExcelFileWithOrder();


}

zamGenerator::~zamGenerator()
{
    if(db.open())
        db.close();
    delete ui;
}

void zamGenerator::openSqlDataBase()
{
    //podłaczenie bazy danych
    db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("localhost");
    db.setUserName("root");
    db.setPassword("");
    db.setDatabaseName("zabawki");

    if(db.open())
        ui->infoListWidget->addItem("Baza danych podłączona");
    else
    {
        ui->infoListWidget->addItem("Nie udało się podłączyć bazy danych");
    }
    /*
    QString kod = "748415";
    QSqlQueryModel *query = new QSqlQueryModel;
    query->setQuery("SELECT ID,EAN,KOD,zamowienie,opis FROM `towary` WHERE kod = "+kod);
    ui->tableViewSQL->setModel(query);
    */
}

//wybieramy arkusz excel z zamówieniem, jak nie zostanie wybrany żaden plik, to uruchamiamy przycisk do zaczytania
void zamGenerator::openExcelFileWithOrder()
{
    excelFilePath = QFileDialog::getOpenFileName(this,"Wybierz arkusz Excel z listą towarów do zamówienia, "
                                                      "pamiętaj aby nie było polskich znaków po drodze!!!!",
                                                 "C:/","*.xls *.xlsx");
    if(excelFilePath.isEmpty())
    {
        ui->infoListWidget->addItem("Nie został wybrany plik excel z zamówieniem!!!");
        ui->setExceFileToOrder->setVisible(true);
    }
    else
    {
        ui->infoListWidget->addItem("Plik excel z zamówieniem wybrany!");
        loadOrderListFromExcel();
    }

}


void zamGenerator::on_setExceFileToOrder_clicked()
{
    openExcelFileWithOrder();
    if(!excelFilePath.isEmpty())
        ui->setExceFileToOrder->setVisible(false);
}

void zamGenerator::loadOrderListFromExcel()
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


                while (cellType!=CELLTYPE_EMPTY)
                {
                    switch (cellType)
                      {
                                           case CELLTYPE_EMPTY: break;
                                           case CELLTYPE_NUMBER:
                                           {
                                               double d = sheet->readNum(row, col);
                                               kod.setNum(d,'g',13);
                                               listaDoZamowienia.append(kod);
                                               break;
                                           }
                                           case CELLTYPE_STRING:
                                           {
                                               kodString = sheet->readStr(row, col);
                                               QString A = kodString.c_str();
                                               listaDoZamowienia.append(A);
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
