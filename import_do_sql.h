#ifndef IMPORT_DO_SQL_H
#define IMPORT_DO_SQL_H

#include <QDialog>
#include <zamgenerator.h>
#include <QDir>

namespace Ui {
class import_do_sql;
}

class import_do_sql : public QDialog
{
    Q_OBJECT

public:
    explicit import_do_sql(QWidget *parent = nullptr);
    ~import_do_sql();

private slots:
    void on_excel_order_to_import_clicked();

    void on_upload_to_sql_start_clicked();

private:
    Ui::import_do_sql *ui;
    QSqlDatabase db;
    QString excelFilePath;
    QStringList rowFromExcel;
    void openSqlDataBase();
    void dataFromExelFile(); // zaczytanie danych z arkusza excel
    void insert_into_sql_DB(int a);  //wstawianie nowego rekordu do BD
    QString textFile(const char *x);
    void readSheet(int x);
    int rowNumberInExcel();
};

#endif // IMPORT_DO_SQL_H
