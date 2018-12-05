#include "zamowienia.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    setlocale(LC_ALL,"");
    Zamowienia w;
    w.show();

    return a.exec();
}
