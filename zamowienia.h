#ifndef ZAMOWIENIA_H
#define ZAMOWIENIA_H

#include <QMainWindow>
#include <QCursor>
#include <QString>
#include <string.h>
#include "libxl.h"
#include <QDialog>
#include <zamgenerator.h>
#include <import_do_sql.h>

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
    void on_makeOrderButton_clicked();

    void on_import_to_sql_pushButton_clicked();

private:
    Ui::Zamowienia *ui;
    QPoint dragPosition;
    QString excelFilePath;
    QStringList listaDoZamowienia;
};

#endif // ZAMOWIENIA_H
