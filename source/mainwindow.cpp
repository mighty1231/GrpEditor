#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtCore>
#include <QtWidgets>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    grpImage = NULL;

    /* load data files */
    data = Data::getInstance();
    loadData();

    ui->grpImageScrollArea->setBackgroundRole(QPalette::Dark);
    connect(ui->act_open_grp, SIGNAL(triggered()), this, SLOT(loadGrp()));
    connect(ui->act_pallete, SIGNAL(toggled(bool)), this, SLOT(openPallete()));
    connect(data, SIGNAL(grpChanged(int, int, char *)),
            this, SLOT(updatePixelData(int, int, char *)));
    connect(data, SIGNAL(colorTableChanged(QVector<QRgb>)),
            this, SLOT(updatePallete(QVector<QRgb>)));

    connect(ui->frameListWidget, SIGNAL(currentRowChanged(int)),
            data, SLOT(setGrpIndex(int)));

    show();
    palleteWindow = NULL;
    openPallete();
}

void MainWindow::loadData()
{
    loadWpe();
    loadMapping();
    loadRemapping();
    loadColorCycling();
    data->updateColorTable();
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

void MainWindow::loadColorCycling()
{
    data->appendColorCycling(ColorCycling::loadDefault());
    QDir dir("data/colorcycling/");
    dir.setFilter(QDir::Files | QDir::NoDotAndDotDot);
    QStringList filters;
    filters << "*.colorcycling";
    dir.setNameFilters(filters);
    QDirIterator iter(dir);
    while (iter.hasNext()) {
        iter.next();
        ColorCycling *colorCycling = ColorCycling::load(iter.fileInfo().completeBaseName(),
                             iter.filePath());
        if (colorCycling == NULL) {
            /* fail */
        } else {
            data->appendColorCycling(colorCycling);
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

    Grp *new_grp = Grp::load(fname);
    if (new_grp == NULL) {
        QMessageBox::critical(this, "Error", "Error happened in grp loading");
        return;
    }

    Grp *old_grp = data->getGrp();
    if (old_grp != NULL) {
        delete old_grp;
        ui->frameListWidget->clear();
    }
    for (int i=0; i<new_grp->getFrameCount(); i++) {
        ui->frameListWidget->addItem(QString::asprintf("Frame #%u", i));
    }
    data->setGrp(new_grp);
}

void MainWindow::saveGrp()
{

}

void MainWindow::openPallete()
{
    if (palleteWindow == NULL) {
        palleteWindow = new PalleteWindow(this);
        connect(palleteWindow, SIGNAL(closing()),
                this, SLOT(palleteClosed()));
        palleteWindow->show();
    } else {
        palleteWindow->close();
    }
}

void MainWindow::palleteClosed()
{
    palleteWindow = NULL;
}

void MainWindow::updatePixelData(int width, int height, char *frame)
{
    qDebug() << "updatePixelData()";
    if (grpImage == NULL) {
        grpImage = new QImage(width, height, QImage::Format_Indexed8);
        grpImage->setColorTable(data->getColorTable());
    } else if ((grpImage->width() != width)
               || (grpImage->height() != height)) {
        delete grpImage;
        grpImage = new QImage(width, height, QImage::Format_Indexed8);
        grpImage->setColorTable(data->getColorTable());
    }
    for (int i=0; i<height; i++) {
        memcpy(grpImage->scanLine(i), frame+i*width, width);
    }
    grpPixmap.convertFromImage(*grpImage);
    ui->grpImageLabel->setPixmap(grpPixmap);
}

void MainWindow::updatePallete(QVector<QRgb> colorTable)
{
    qDebug() << "updatePallete";
    if (grpImage == NULL)
        return;
    grpImage->setColorTable(colorTable);
    grpPixmap.convertFromImage(*grpImage);
    ui->grpImageLabel->setPixmap(grpPixmap);
}

MainWindow::~MainWindow()
{
    delete ui;
}
