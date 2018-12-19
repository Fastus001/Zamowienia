#ifndef ZAMGENERATOR_H
#define ZAMGENERATOR_H

#include <QDialog>
#include <QtSql/QSql>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QSqlQueryModel>
#include <QFileDialog>
#include <QMessageBox>
#include "libxl.h"

namespace Ui {
class zamGenerator;
}

class zamGenerator : public QDialog
{
    Q_OBJECT

public:
    explicit zamGenerator(QWidget *parent = nullptr);
    ~zamGenerator();

private slots:


    void on_setExceFileToOrder_clicked();
    void loadOrderListFromExcel();

private:
    Ui::zamGenerator *ui;
    QSqlDatabase db;
    QStringList listaDoZamowienia;
    QString excelFilePath;
    void openSqlDataBase();
    void openExcelFileWithOrder();
};

#endif // ZAMGENERATOR_H
