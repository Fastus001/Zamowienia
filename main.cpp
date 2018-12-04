#include "zamowienia.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Zamowienia w;
    w.show();

    return a.exec();
}
