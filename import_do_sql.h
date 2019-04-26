#ifndef IMPORT_DO_SQL_H
#define IMPORT_DO_SQL_H

#include <QDialog>
#include <zamgenerator.h>
#include <QDir>
#include <string>

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

    void on_add_ID_in_excel_file_clicked();

    void on_update_BD_button_clicked();

private:
    Ui::import_do_sql *ui;
    QSqlDatabase db;
    QString excelFilePath;
    QStringList rowFromExcel;
    void openSqlDataBase();
    void dataFromExelFile(int b = 0); // zaczytanie danych z arkusza excel
    void insert_into_sql_DB(int a);  //wstawianie nowego rekordu do BD,
    void update_sql_DB_record(int a);
    bool check_DB_for_same_item(int a);
    bool check_DB_for_same_item_by_ID(int a);
    //zaczytywanie z excela tylko szybsze, aby nie otwierać i zamykać po każdym rekordzie
    void read_from_excel_fast(int number_of_rows,int start, QString file_name,int ex_col); //versja do zaczytania dodatkowej wybranej kolumny
    void read_from_excel_fast_to_add_ID(int number_of_rows,int start);
    void add_Id_from_DB();
    void write_Id_to_excel(QStringList & sList);
    QString textFile(const char *x);
    QString read_from_excel_switch(libxl::Sheet *sh, libxl::CellType ct,int r,int c);
    void readSheet(int x, int b = 0);
    int rowNumberInExcel();
    QString file_name(QString fn);
    libxl::Book* open_Book(QString qs);
    void get_file_address();
    QString set_query_string(int a);
    bool check_id_in_excel();
};

#endif // IMPORT_DO_SQL_H
