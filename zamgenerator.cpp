#include "zamgenerator.h"
#include "ui_zamgenerator.h"

zamGenerator::zamGenerator(QWidget *parent) :
    QDialog(parent, Qt::FramelessWindowHint),
    ui(new Ui::zamGenerator)
{
    ui->setupUi(this);
    ui->setExceFileToOrder->setVisible(false);
    ui->backButton->setVisible(false);
    indexOrderList = 0;
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
        //ui->infoListWidget->addItems(listaDoZamowienia);
        displaySqlQuery(indexOrderList);

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
    //setlocale(LC_ALL,"");
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
            book = xlCreateBookA();

        }

        QByteArray ba = excelFilePath.toUtf8();
        if(book->load(ba.data()))
        {
            Sheet *sheet = book->getSheet(0);

            if(sheet)
            {
                int row = 15;
                int col = 1;
                QString kod;
                std::string kodString,cartonNumber;

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
                                               appendListFunction(kod,col);
                                               //listaDoZamowienia.append(kod);
                                               break;
                                           }
                                           case CELLTYPE_STRING:
                                           {
                                               kodString = sheet->readStr(row, col);
                                               QString A = kodString.c_str();
                                               appendListFunction(A,col);
                                               //listaDoZamowienia.append(A);
                                               break;
                                           }
                                           case CELLTYPE_BOOLEAN: break;
                                           case CELLTYPE_BLANK:break;
                                           case CELLTYPE_ERROR:break;
                      }
                    if(col==1)
                        col+=4;
                    else{
                        col = 1;
                        row++;
                    }

                   cellType = sheet->cellType(row, col);
                }
            }
            book->release();
        }
    }
}

void zamGenerator::displaySqlQuery(int i)
{
        QString kod = listaDoZamowienia.at(i);
        QSqlQueryModel *query = new QSqlQueryModel;
        ui->lineItemNumber->setText(kod);
        ui->lineCtnsOrder->setText(ctnToOrder.at(i));

        query->setQuery("SELECT * FROM `towary` WHERE kod = "+kod);

        ui->tableViewSQL->setModel(query);
        ui->tableViewSQL->setColumnWidth(0,30);
        ui->tableViewSQL->setColumnWidth(2,80);
        ui->tableViewSQL->setColumnWidth(3,45);
        ui->tableViewSQL->setColumnWidth(4,30);
        ui->tableViewSQL->setColumnWidth(5,30);
        ui->tableViewSQL->setColumnWidth(6,40);
        ui->tableViewSQL->setColumnWidth(7,100);
        ui->tableViewSQL->setColumnWidth(8,220);
        ui->tableViewSQL->setColumnWidth(9,220);
        ui->tableViewSQL->setColumnWidth(10,220);


}
//pomijamy konkretny rekord
void zamGenerator::on_nextButton_clicked()
{
    ++indexOrderList;
    if(indexOrderList>=1)
        ui->backButton->setVisible(true);
    if(indexOrderList<listaDoZamowienia.size())
    {
        displaySqlQuery(indexOrderList);
        if(indexOrderList==listaDoZamowienia.size()-1)
           ui->nextButton->setVisible(false);
    }
}
//cofamy do wcześniejszej pozycji
void zamGenerator::on_backButton_clicked()
{
    --indexOrderList;
    ui->nextButton->setVisible(true);
    if (indexOrderList==0)
        ui->backButton->setVisible(false);
    if(indexOrderList>=0)
        displaySqlQuery(indexOrderList);


}

void zamGenerator::on_tableViewSQL_clicked(const QModelIndex &index)
{
    QString val = ui->tableViewSQL->model()->data(index).toString();

    QSqlQuery nQuery("SELECT * FROM `towary` WHERE ID= "+val);

    if(nQuery.exec())
    {
        while(nQuery.next())
        {
            selectedIdList.clear();
            ui->infoListWidget->clear();

            for(int i=0;i<=9;++i)
               selectedIdList.append(nQuery.value(i).toString());

            QString rawNumberString;
            //dodanie numeru pozycji do listy argumentów, przyda się przy sprawdzeniu czy dobry rekord wysłamy do arkusza z zamówieniem
            selectedIdList.append(rawNumberString.setNum(indexOrderList));
            ui->infoListWidget->addItems(selectedIdList);
        }
        //db2.close();
    }
}

void zamGenerator::on_okButton_clicked()
{
    if(!selectedIdList.isEmpty())
    {
        if(indexOrderList==selectedIdList.at(10).toInt())
        {
            uploadToExcelOrder(selectedIdList);
            on_nextButton_clicked();
        }else
            QMessageBox::information(this,"!!!!!!", "Zaznaczony rekord nie pokrywa się z szukaną pozycją");
    }

}

void zamGenerator::uploadToExcelOrder(QStringList sList)
{
        setlocale(LC_ALL,"");
        using namespace libxl;
        Book* book;
        if(excelFilePath.contains(".xlsx"))
        {
            book = xlCreateXMLBook();
        }else{
            //book = xlCreateBook();
            book = xlCreateBookA();

        }
        QByteArray ba = excelFilePath.toUtf8();
        if(book->load(ba.data()))
        {
            Sheet *sheet = book->getSheet(0);

            if(sheet)
            {
                int row = 15+sList.at(10).toInt();
                int col = 0;
                QByteArray test = sList.at(1).toLocal8Bit();
                sheet->writeStr(row,col,test.data());
                int qty;
                double cena,vol;
                cena = sList.at(3).toDouble();
                sheet->writeNum(row,col+2,cena);
                qty = sList.at(4).toInt();
                sheet->writeNum(row,col+3,qty);
                vol = sList.at(5).toDouble();
                sheet->writeNum(row,col+4,vol);
                test = sList.at(7).toLocal8Bit();
                sheet->writeStr(row,col+6,test.data());
                test = sList.at(8).toLocal8Bit();
                sheet->writeStr(row,col+7,test.data());
                test = sList.at(9).toLocal8Bit();
                sheet->writeStr(row,col+8,test.data());


                book->save(ba.data());
        }
        book->release();
        }
}

void zamGenerator::appendListFunction(QString &s, int a)
{

    if(a == 1)
        listaDoZamowienia.append(s);
    else
        ctnToOrder.append(s);
}
/*
void zamGenerator::on_lineCtnsOrder_textChanged(const QString &arg1)
{
    selectedIdList.replaceInStrings(selectedIdList.at(0),arg1);
    ui->infoListWidget->clear();
    ui->infoListWidget->addItems(selectedIdList);


}
*/

void zamGenerator::on_updateSelectedIdList_clicked()
{
    if(!selectedIdList.isEmpty())
    {
    selectedIdList.replaceInStrings(selectedIdList.at(1),ui->lineCtnsOrder->text());
    ui->infoListWidget->clear();
    ui->infoListWidget->addItems(selectedIdList);
    }

}
