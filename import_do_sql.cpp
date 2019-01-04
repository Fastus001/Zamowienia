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
    delete ui;
}
