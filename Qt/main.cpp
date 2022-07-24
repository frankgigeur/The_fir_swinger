#include "mainwindow.h"
#include "accueil.h"
#include <QApplication>

int main(int argc, char *argv[])
{

    QApplication a(argc, argv);
    //MainWindow w(delayMs);
    Accueil w;
    w.show();
    return a.exec();
}
