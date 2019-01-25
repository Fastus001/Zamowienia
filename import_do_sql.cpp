#include "import_do_sql.h"
#include "ui_import_do_sql.h"

import_do_sql::import_do_sql(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::import_do_sql)
{
    ui->setupUi(this);
}

import_do_sql::~import_do_sql()
{
    if(db.open())
        db.close();
    delete ui;
}

void import_do_sql::openSqlDataBase()
{
    //podłaczenie bazy danych
    db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("localhost");
    db.setUserName("root");
    db.setPassword("");
    db.setDatabaseName("zabawki");

    if(db.open())
        ui->listWidget->addItem(tr("Baza danych podłączona"));
    else
    {
        ui->listWidget->addItem(tr("Nie udało się podłączyć bazy danych"));
    }
}

void import_do_sql::dataFromExelFile()
{
    if(excelFilePath.isEmpty())
    {
        QMessageBox::information(this, "Brak pliku excel","Nie został wybrany arkusz excel z listą produktów do zamówienia!!");
    }else{


        //najpierw trzeba zliczyć ilość wierszy w arkuszu
        int number_of_row = rowNumberInExcel();
        QString s; //iloś wierszy z kodami towarów w arkuszu
        ui->listWidget->addItem(s.setNum(number_of_row));
        //zaczytać te wiersze, wiedząc ile mamy wierszy
        readSheet(number_of_row);

    }
}
//zaczytywanie polski znaków !!
QString import_do_sql::textFile(const char *x)
{
    QByteArray encodedString(x);
    QTextCodec *codec = QTextCodec::codecForName("Windows-1250");
    QString string = codec->toUnicode(encodedString);

    return string;
}

void import_do_sql::readSheet(int x)
{
    using namespace libxl;
    int row = 16;
    int col = 0;
    QByteArray ba = excelFilePath.toUtf8();
    QString kod;
    for(int k=0;k<x;k++)
    {
        Book* book;
        if(excelFilePath.contains(".xlsx"))
        {
            book = xlCreateXMLBook();
        }else{
            book = xlCreateBookA();
        }

        if(book->load(ba.data()))
        {
            Sheet *sheet = book->getSheet(0);

            if(sheet)
            {
                CellType cellType = sheet->cellType(row, col);
                switch (cellType)
                  {
                                      case CELLTYPE_EMPTY:break;
                                      case CELLTYPE_NUMBER:
                                       {
                                           double d = sheet->readNum(row, col);
                                           kod.setNum(d,'g',13);
                                           rowFromExcel.append(kod);
                                           break;
                                       }
                                       case CELLTYPE_STRING:
                                       {
                                           const char *b = sheet->readStr(row, col);
                                           QString A(textFile(b));
                                           rowFromExcel.append(A);
                                           break;
                                       }
                                       case CELLTYPE_BOOLEAN: break;
                                       case CELLTYPE_BLANK:
                                        {
                                        rowFromExcel.append("NULL");
                                        break;
                                        }
                                       case CELLTYPE_ERROR:break;
                  }
            }
            book->release();
        ui->listWidget->addItems(rowFromExcel);
        }

        }
        rowFromExcel.append(dir.dirName());
}

//funkcja zliczające liczbę wierszy do zaczytania w arkusze excel, potrzebne bo operujemy na wersji demo biblioteki libslx
int import_do_sql::rowNumberInExcel()
{
    using namespace libxl;
    int size = 0;
    int row = 16;
    int col = 2;
    Book* book;
        if(excelFilePath.contains(".xlsx"))
        {
            book = xlCreateXMLBook();
        }else{
            book = xlCreateBookA();
        }

        QByteArray ba = excelFilePath.toUtf8();
        if(book->load(ba.data()))
        {
            Sheet *sheet = book->getSheet(0);
            if(sheet)
            {
                CellType cellType = sheet->cellType(row, col);

                for (col = 0; col<9;++col)
                {
                    cellType = sheet->cellType(row, col);
                    switch (cellType)
                      {
                       case CELLTYPE_EMPTY:break;
                       case CELLTYPE_NUMBER: size++; break;
                       case CELLTYPE_STRING: size++; break;
                       case CELLTYPE_BOOLEAN: break;
                       case CELLTYPE_BLANK: break;
                       case CELLTYPE_ERROR: break;
                      }
                }
            }

        }
        return size;
}

void import_do_sql::on_excel_order_to_import_clicked()
{
    excelFilePath = QFileDialog::getOpenFileName(this,"Wybierz arkusz Excel z listą towarów do zamówienia, "
                                                      "pamiętaj aby nie było polskich znaków po drodze!!!!",
                                                 "C:/","*.xls *.xlsx");

    ui->listWidget->addItem(excelFilePath);
    QDir dir = excelFilePath;
    QString file_name = dir.dirName();
    ui->listWidget->addItem(file_name);

    dataFromExelFile();
}
