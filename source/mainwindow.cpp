#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtCore>
#include <QtWidgets>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    /* load data files */
    data = Data::getInstance();
    loadData();

    palleteWindow = new PalleteWindow(this);

    connect(ui->actionOpen_grp, SIGNAL(triggered()), this, SLOT(loadGrp()));

    show();
    palleteWindow->show();
}

void MainWindow::loadData()
{
    loadWpe();
    loadMapping();
    loadRemapping();
}

void MainWindow::loadWpe()
{

    /* wpe */
    data->appendWpe(Wpe::loadDefault());
    QDir dir("data/wpe/");
    dir.setFilter(QDir::Files | QDir::NoDotAndDotDot);
    QStringList filters;
    filters << "*.wpe";
    dir.setNameFilters(filters);
    QDirIterator iter(dir);
    while (iter.hasNext()) {
        iter.next();
        Wpe *wpe = Wpe::load(iter.fileInfo().completeBaseName(),
                             iter.filePath());
        if (wpe == NULL) {
            /* fail */
        } else {
            data->appendWpe(wpe);
        }
    }
}

void MainWindow::loadMapping()
{
    data->appendMapping(Mapping::loadDefault());
    QDir dir("data/mapping/");
    dir.setFilter(QDir::Files | QDir::NoDotAndDotDot);
    QStringList filters;
    filters << "*.mapping";
    dir.setNameFilters(filters);
    QDirIterator iter(dir);
    while (iter.hasNext()) {
        iter.next();
        Mapping *mapping = Mapping::load(iter.fileInfo().completeBaseName(),
                             iter.filePath());
        if (mapping == NULL) {
            /* fail */
        } else {
            data->appendMapping(mapping);
        }
    }
}

void MainWindow::loadRemapping()
{
    data->appendRemapping(Remapping::loadDefault());
    QDir dir("data/remapping/");
    dir.setFilter(QDir::Files | QDir::NoDotAndDotDot);
    QStringList filters;
    filters << "*.remapping";
    dir.setNameFilters(filters);
    QDirIterator iter(dir);
    while (iter.hasNext()) {
        iter.next();
        Remapping *remapping = Remapping::load(iter.fileInfo().completeBaseName(),
                             iter.filePath());
        if (remapping == NULL) {
            /* fail */
        } else {
            data->appendRemapping(remapping);
        }
    }
}

void MainWindow::loadGrp()
{
    QString fname = QFileDialog::getOpenFileName(
                this,
                tr("Open File"),
                tr(""),
                tr("Grp files (*.grp)"));
    qDebug() << fname;

    Grp *grp = Grp::load(fname);
    if (grp != NULL) {
        data->setGrp(grp);
    }
}

void MainWindow::saveGrp()
{

}

MainWindow::~MainWindow()
{
    delete ui;
}
