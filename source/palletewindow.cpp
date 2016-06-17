#include "palletewindow.h"
#include "ui_palletewindow.h"

PalleteWindow::PalleteWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::PalleteWindow)
{
    ui->setupUi(this);
}

PalleteWindow::~PalleteWindow()
{
    delete ui;
}
