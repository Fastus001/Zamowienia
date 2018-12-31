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
    void displaySqlQuery(int i);

    void on_nextButton_clicked();

    void on_backButton_clicked();

    void on_tableViewSQL_clicked(const QModelIndex &index);

    void on_okButton_clicked();

    void uploadToExcelOrder(QStringList sList);

    //funkcja która w zależności od numeru kolumny przypisuje strina albo do listaDoZamowienia albo do ctnToOrder
    void appendListFunction(QString &s, int a);

    //void on_lineCtnsOrder_textChanged(const QString &arg1);

    void on_updateSelectedIdList_clicked();

private:
    Ui::zamGenerator *ui;
    QSqlDatabase db;
    QStringList listaDoZamowienia;
    QStringList ctnToOrder;
    QStringList selectedIdList;
    QString excelFilePath;
    int indexOrderList;
    void openSqlDataBase();
    void openExcelFileWithOrder();
};

#endif // ZAMGENERATOR_H
