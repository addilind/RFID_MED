#include "homestation.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Homestation home;
    home.show();

    return a.exec();
}
