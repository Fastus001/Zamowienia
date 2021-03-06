#include "import_do_sql.h"
#include "ui_import_do_sql.h"

import_do_sql::import_do_sql(QWidget *parent) :
    QDialog(parent, Qt::FramelessWindowHint),
    ui(new Ui::import_do_sql)
{
    ui->setupUi(this);
    ui->add_ID_in_excel_file->setVisible(false);
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

void import_do_sql::dataFromExelFile(int b)
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
        readSheet(number_of_row,b);
    }
}

void import_do_sql::insert_into_sql_DB(int a)
{
    QSqlQuery query;
    int x = a*10;
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
//uaktualnienie rekordów w całym zamówieniu, nie jest sprawdzane czy coś rzeczywiście się zmieniło a tylko czy jest towar o takim ID
void import_do_sql::update_sql_DB_record(int a)
{
    QSqlQuery query;
    int x = a*11;

    query.prepare(set_query_string(a));
    //ui->listWidget->addItem(set_query_string(a));

    if(query.exec())
        ui->listWidget->addItem("Produkt "+rowFromExcel.at(x+1)+" uaktualniony w BD");
    else {
        ui->listWidget->addItem("nie udało się wstawić do BD towaru o numerze "+rowFromExcel.at(x+1));
    }
}

bool import_do_sql::check_DB_for_same_item(int a)
{
    int x = a*10;
    QString zapytanie("SELECT * FROM towary where kod = '"+rowFromExcel.at(x+1)+"' and orderDate = '"+rowFromExcel.at(x+9)+"'");
    QSqlQuery query(zapytanie);

    if(query.size()>0)
        return true;
    else
        return false;
}

bool import_do_sql::check_DB_for_same_item_by_ID(int a)
{
    int x = a*11;
    QString zapytanie("SELECT * FROM towary where ID = "+rowFromExcel.at(x+9));
    QSqlQuery query(zapytanie);

    if(query.size()>0)
        return true;
    else
        return false;
}

void import_do_sql::read_from_excel_fast(int number_of_rows, int start, QString file_name, int ex_col)
{
    using namespace libxl;
    QString kod;
    QByteArray ba = excelFilePath.toUtf8();

    int row = 16;
    int col = 0;
    int k = 0;
    int dist = 30; //liczba rekordów zaczytywanych w jednym cyklu (ograniczone przez libslx)
    Book* book;
    book = open_Book(excelFilePath);
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
                    rowFromExcel.append(read_from_excel_switch(sheet, cellType,row+k+start,col+var));
                }
                if(ex_col>0)
                {
                    cellType = sheet->cellType(row+k+start, col+ex_col);
                    rowFromExcel.append(read_from_excel_switch(sheet, cellType,row+k+start,col+ex_col));
                }
                rowFromExcel.append(file_name);
            }
            book->release();
            if(start<number_of_rows)
                return read_from_excel_fast(number_of_rows,start+dist,file_name,ex_col);
        }
    }

}

void import_do_sql::read_from_excel_fast_to_add_ID(int number_of_rows, int start)
{
    using namespace libxl;
    QString kod;
    QByteArray ba = excelFilePath.toUtf8();


    int row = 16;
    int col = 1;
    int k = 0;
    int dist = 40; //liczba rekordów zaczytywanych w jednym cyklu (ograniczone przez libslx)
    Book* book;
    book = open_Book(excelFilePath);
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

                cellType = sheet->cellType(row+k+start, col);
                rowFromExcel.append(read_from_excel_switch(sheet, cellType,row+k+start,col));
            }

        }
        book->release();
        if(start<=number_of_rows)
            return read_from_excel_fast_to_add_ID(number_of_rows,start+dist);
    }

}
//pobranie ID z BD do konkretnego numeru i zamówienia
void import_do_sql::add_Id_from_DB()
{
    QString fn(file_name(excelFilePath));
    QStringList result;
    for (int i = 0;i<rowFromExcel.size();++i)
    {
       QString zapytanie("SELECT ID FROM towary where kod = '"+rowFromExcel.at(i)+"' and orderDate = '"+fn+"'");
       QSqlQuery query(zapytanie);
       QString q;
       if(query.size()>0)
       {
            result.append(rowFromExcel.at(i));
            while(query.next())
            {
               q= query.value(0).toString();
            }
            result.append(q);
       }
    }
    ui->listWidget->clear();
    ui->listWidget->addItems(result);
    write_Id_to_excel(result);
}
//zapisanie w arkuszu z zamówienie ID towaru z bazy danych, nie zapisze na raz wiecej niż 300 id ze względu na demo LIBXL
void import_do_sql::write_Id_to_excel(QStringList &sList)
{
    using namespace libxl;
    int row = 16;
    int col = 1;
    QString testString;

    Book* book;
    book = open_Book(excelFilePath);
    QByteArray test;
    QByteArray ba = excelFilePath.toUtf8();
    if(book->load(ba.data()))
    {
        Sheet *sheet = book->getSheet(0);
        CellType cellType;

        if(sheet)
        {
            for(int i = 0; i<sList.size();i+=2)
            {
                    int x = i/2;
                    cellType = sheet->cellType(row+x, col);
                    testString = read_from_excel_switch(sheet,cellType,row+x,col);
                    if(testString==sList.at(i))
                    {
                        test = sList.at(i+1).toLocal8Bit();
                        sheet->writeStr(row+x,col+17,test.data());

                    }
            }
            book->save(ba.data());
        }
    book->release();
    }

}


//zaczytywanie polskich znaków !!
QString import_do_sql::textFile(const char *x)
{
    QByteArray encodedString(x);
    QTextCodec *codec = QTextCodec::codecForName("Windows-1250");
    QString string = codec->toUnicode(encodedString);
    return string;
}
//zwrot wartości z określonego pola z arkusza excel, switch
QString import_do_sql::read_from_excel_switch(libxl::Sheet *sh, libxl::CellType ct, int r, int c)
{
    using namespace libxl;
    QString text;
    switch (ct)
      {
         case CELLTYPE_EMPTY:break;
         case CELLTYPE_NUMBER:
                           {
                               double d = sh->readNum(r, c);
                               text.setNum(d,'g',13);
                               return text;
                           }
         case CELLTYPE_STRING:
                           {
                               const char *b = sh->readStr(r, c);
                               QString A(textFile(b));
                               return A;
                           }
         case CELLTYPE_BOOLEAN: break;
         case CELLTYPE_BLANK:
                           {
                            text = "NULL";
                            return text;
                           }
         case CELLTYPE_ERROR:break;
      }
    return text;
}

void import_do_sql::readSheet(int x, int b)
{
    rowFromExcel.clear();
    read_from_excel_fast(x,0,file_name(excelFilePath),b);
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
    book = open_Book(excelFilePath);



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

QString import_do_sql::file_name(QString fn)
{
    QDir dir(fn);
    QString test = dir.dirName();
    //usunięcie rozszerzenia z nazwy pliku
    int a = test.indexOf(".xls");
    test.truncate(a);
    return test;
}

libxl::Book *import_do_sql::open_Book(QString qs)
{
    if(qs.contains(".xlsx"))
    {
        return xlCreateXMLBook();
    }else{
        return xlCreateBook();
    }
}

void import_do_sql::get_file_address()
{
    excelFilePath = QFileDialog::getOpenFileName(this,"Wybierz arkusz Excel z listą towarów do zamówienia, "
                                                      "pamiętaj aby nie było polskich znaków po drodze!!!!",
                                                 "C:/","*.xls *.xlsx");
}

QString import_do_sql::set_query_string(int a)
{
    int x = a*11;

    QString query = "UPDATE towary SET EAN = '"+rowFromExcel.at(x)+"',kod = '"+rowFromExcel.at(x+1);
    query += "', cena = "+rowFromExcel.at(x+2);
    query += ", pakowanie = "+rowFromExcel.at(x+3);
    query += ", volumen = "+rowFromExcel.at(x+4);
    query += ", zamowienie = '"+rowFromExcel.at(x+5);
    query += "', opis = '"+rowFromExcel.at(x+6);
    query += "', uwagi = '"+rowFromExcel.at(x+7);
    query += "', etykiety = '"+rowFromExcel.at(x+8);
    query += "', orderDate = '"+rowFromExcel.at(x+10);
    query += "' WHERE ID = "+rowFromExcel.at(x+9);
    return query;
}

bool import_do_sql::check_id_in_excel()
{
    for (int i = 0; i<rowFromExcel.size();i+=11)
    {
            if(rowFromExcel.at(i+9)=="NULL")
               return false;
    }
    return true;
}

void import_do_sql::on_excel_order_to_import_clicked()
{
    get_file_address();
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
    ui->add_ID_in_excel_file->setVisible(true);
}

void import_do_sql::on_add_ID_in_excel_file_clicked()
{
    rowFromExcel.clear(); // czyścimy listę z wcześnijszych danych
    read_from_excel_fast_to_add_ID(rowNumberInExcel(),0);
    ui->listWidget->clear();
    //ui->listWidget->addItems(rowFromExcel);
    add_Id_from_DB();
}

void import_do_sql::on_update_BD_button_clicked()
{
    get_file_address();
    dataFromExelFile(18);

    if(check_id_in_excel())
    {
        if(!db.open())
            openSqlDataBase();
        int size = rowFromExcel.size();
        QString rozmiar;
        rozmiar.setNum(size);
        ui->listWidget->addItem(rozmiar);
        if(!rowFromExcel.isEmpty())
        {
            for (int i=0;i<size/11;++i)
            {
                if(check_DB_for_same_item_by_ID(i))
                {
                    update_sql_DB_record(i);
                }
                else {
                    QMessageBox::information(this, "Mamy problem", "Nie ma w bazie danych towaru o ID = "+rowFromExcel.at((i*11)+9));
                    ui->listWidget->addItem(rowFromExcel.at((i*11)+1)+" Nie udało się uaktualnić!!!");
                }
            }
        }
    }else {
    QMessageBox::information(this, "Brak ID!!", "Brak numeru ID w jakieś pozycji w arkuszu excel!");
    }
}
