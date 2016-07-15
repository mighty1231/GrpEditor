#include "widget/mainwindow.h"
#include "widget/palletewindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    return a.exec();
}
