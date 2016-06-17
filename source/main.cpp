#include "mainwindow.h"
#include "palletewindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    return a.exec();
}
