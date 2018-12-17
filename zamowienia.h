#ifndef ZAMOWIENIA_H
#define ZAMOWIENIA_H

#include <QMainWindow>
#include <QCursor>
#include <QtSql/QSql>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QString>
#include <string.h>
#include "libxl.h"
#include <QDialog>
#include <QSqlQueryModel>
#include <QTableView>
#include <zamgenerator.h>

namespace Ui {
class Zamowienia;
}

class Zamowienia : public QMainWindow
{
    Q_OBJECT

public:
    explicit Zamowienia(QWidget *parent = nullptr);
    ~Zamowienia() override;

protected:
    void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;

private slots:
    void on_connectToSql_clicked();

    void on_exelFilePathButton_clicked();

    void on_searchItemsInSqlButton_clicked();


    void on_tableView_clicked(const QModelIndex &index);

    void on_makeOrderButton_clicked();

private:
    Ui::Zamowienia *ui;
    QPoint dragPosition;
    QString excelFilePath;
    QStringList listaDoZamowienia;
};

#endif // ZAMOWIENIA_H
