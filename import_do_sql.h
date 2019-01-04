#ifndef IMPORT_DO_SQL_H
#define IMPORT_DO_SQL_H

#include <QDialog>

namespace Ui {
class import_do_sql;
}

class import_do_sql : public QDialog
{
    Q_OBJECT

public:
    explicit import_do_sql(QWidget *parent = nullptr);
    ~import_do_sql();

private:
    Ui::import_do_sql *ui;
};

#endif // IMPORT_DO_SQL_H
