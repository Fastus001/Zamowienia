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
        //wyczyszczenie info, jeżeli zaczytujemy kolejny dokument
        ui->listWidget->clear();
        //najpierw trzeba zliczyć ilość wierszy w arkuszu
        int number_of_row = rowNumberInExcel();
        QString s; //iloś wierszy z kodami towarów w arkuszu
        ui->listWidget->addItem(s.setNum(number_of_row));
        //pokazuje ściężkę skąd plik jest zaczytywany,
        ui->listWidget->addItem(excelFilePath);
        QDir dir = excelFilePath;
        QString file_name = dir.dirName();
        ui->listWidget->addItem(file_name);

        //zaczytać te wiersze, wiedząc ile mamy wierszy
        readSheet(number_of_row);
    }
}

void import_do_sql::insert_into_sql_DB(int a)
{
    int x = a*10;
    QSqlQuery query;
    query.prepare("INSERT INTO towary (EAN, kod, cena, pakowanie, volumen, zamowienie, opis, uwagi, etykiety, orderDate) "
                  "VALUES (:EAN,:kod,:cena, :pakowanie, :volumen, :zamowienie, :opis, :uwagi, :etykiety, :orderDate)");
    query.bindValue(":EAN",rowFromExcel.at(x));
    query.bindValue(":kod",rowFromExcel.at(x+1));
    query.bindValue(":cena",rowFromExcel.at(x+2));
    query.bindValue(":pakowanie",rowFromExcel.at(x+3));
    query.bindValue(":volumen",rowFromExcel.at(x+4));
    query.bindValue(":zamowienie",rowFromExcel.at(x+5));
    query.bindValue(":opis",rowFromExcel.at(x+6));
    query.bindValue(":uwagi",rowFromExcel.at(x+7));
    query.bindValue(":etykiety",rowFromExcel.at(x+8));
    query.bindValue(":orderDate",rowFromExcel.at(x+9));
    if(query.exec())
        ui->listWidget->addItem("Produkt "+rowFromExcel.at(x+1)+" udało się wgrać do bazy danych");
    else {
        ui->listWidget->addItem("nie udało się wstawić do BD towaru o numerze "+rowFromExcel.at(x+1));
    }
}

bool import_do_sql::check_DB_for_same_item(int a)
{
    int x = a*10;
    QString zapytanie("SELECT * FROM towary where kod = '"+rowFromExcel.at(x+1)+"' and orderDate = '"+rowFromExcel.at(x+9)+"'");
    QSqlQuery query(zapytanie);
    /*int as = query.size();
    QString As;
    As.setNum(as);
    ui->listWidget->addItem(As);
    ui->listWidget->addItem(zapytanie);*/

    if(query.size()>0)
        return true;
    else
        return false;


}

void import_do_sql::read_from_excel_fast(int number_of_rows, int start, QString file_name)
{
    using namespace libxl;
    QString kod;
    QByteArray ba = excelFilePath.toUtf8();
    int row = 16;
    int col = 0;
    int k = 0;
    int dist = 40; //liczba rekordów zaczytywanych w jednym cyklu (ograniczone przez libslx)
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
            for(k=0;k<dist;++k)
            {
               if(k+start>=number_of_rows)
                   break;
                CellType cellType;
                for (int var = 0; var < 9; ++var)
                {
                    cellType = sheet->cellType(row+k+start, col+var);
                    switch (cellType)
                      {
                                          case CELLTYPE_EMPTY:break;
                                          case CELLTYPE_NUMBER:
                                           {
                                               double d = sheet->readNum(row+k+start, col+var);
                                               kod.setNum(d,'g',13);
                                               rowFromExcel.append(kod);
                                               break;
                                           }
                                           case CELLTYPE_STRING:
                                           {
                                               const char *b = sheet->readStr(row+k+start, col+var);
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
                rowFromExcel.append(file_name);
            }
            book->release();
            if(start<number_of_rows)
                return read_from_excel_fast(number_of_rows,start+dist,file_name);
        }
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
    rowFromExcel.clear();
    QDir dir(excelFilePath);
    QString test = dir.dirName();
    //usunięcie rozszerzenia z nazwy pliku
    int a = test.indexOf(".xls");
    test.truncate(a);
    read_from_excel_fast(x,0,test);

ui->listWidget->addItems(rowFromExcel); //wgranie do widgetu całej listy, później juz nie będzie potrzebne

}

//funkcja zliczające liczbę wierszy do zaczytania w arkusze excel, potrzebne bo operujemy na wersji demo biblioteki libslx
int import_do_sql::rowNumberInExcel()
{
    using namespace libxl;
    int size = 0;
    int row = 16;
    int col = 1; //kolumna z kodami towarowymi
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


                while(cellType!=CELLTYPE_EMPTY){
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
                    ++row;
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
    dataFromExelFile();
}
//funkcja sprawdza czy dany produkt jest już w BD (kod produktu+numer zamówienia), jeżeli nie ma to dodaje do BD
// w innym przypadku nie.
void import_do_sql::on_upload_to_sql_start_clicked()
{
    ui->listWidget->clear();
    openSqlDataBase();
    int size = rowFromExcel.size();
    if(!rowFromExcel.isEmpty())
    {
        for (int i=0;i<size/10;++i)
        {
            if(check_DB_for_same_item(i))
                ui->listWidget->addItem("Produkt "+rowFromExcel.at((i*10)+1)+ " jest już w DB");
            else {
                insert_into_sql_DB(i);
            }
        }
    }
}
