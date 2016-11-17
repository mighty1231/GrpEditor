#include "palletewindow.h"
#include "ui_palletewindow.h"

#include "component/loader.h"
#include "widget/mainwindow.h"
#include "widget/palletetablewidget.h"
#include "component/wpe.h"
#include "component/mapping.h"
#include "component/remapping.h"
#include "component/colorcycling.h"

PalleteWindow::PalleteWindow(MainWindow *mw, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::PalleteWindow)
{
    ui->setupUi(this);

    loader = mw->getLoader();
    QVector<Wpe *> wpes = loader->getWpes();
    QVector<Mapping *> mappings = loader->getMappings();
    QVector<Remapping *> remappings = loader->getRemappings();
    QVector<ColorCycling *> colorCyclings = loader->getColorCyclings();

    for (Wpe *wpe : wpes) {
        ui->combo_wpe->addItem(wpe->getName());
    }
    for (Mapping *map : mappings) {
        ui->combo_map->addItem(map->getName());
    }
    for (Remapping *rem : remappings) {
        ui->combo_rem->addItem(rem->getName());
    }
    for (ColorCycling *cc : colorCyclings) {
        ui->combo_cc->addItem(cc->getName());
    }

    ui->combo_wpe->setCurrentIndex(loader->getWpeIndex());
    ui->combo_map->setCurrentIndex(loader->getMappingIndex());
    ui->combo_rem->setCurrentIndex(loader->getRemappingIndex());
    ui->combo_cc->setCurrentIndex(loader->getColorCyclingIndex());

    connect(ui->combo_wpe, SIGNAL(currentIndexChanged(int)),
            loader, SLOT(setWpeIndex(int)));
    connect(ui->combo_map, SIGNAL(currentIndexChanged(int)),
            loader, SLOT(setMappingIndex(int)));
    connect(ui->combo_rem, SIGNAL(currentIndexChanged(int)),
            loader, SLOT(setRemappingIndex(int)));
    connect(ui->combo_cc, SIGNAL(currentIndexChanged(int)),
            loader, SLOT(setColorCyclingIndex(int)));


    QRgb o = mw->getOverflowedColor();
    ui->overflow_r->setValue(qRed(o));
    ui->overflow_g->setValue(qGreen(o));
    ui->overflow_b->setValue(qBlue(o));
    connect(ui->overflow_r, SIGNAL(valueChanged(int)),
            mw, SLOT(setOverflowR(int)));
    connect(ui->overflow_g, SIGNAL(valueChanged(int)),
            mw, SLOT(setOverflowG(int)));
    connect(ui->overflow_b, SIGNAL(valueChanged(int)),
            mw, SLOT(setOverflowB(int)));

    palleteTableWidget = new PalleteTableWidget(mw, loader, this);
    ui->layout0->addWidget(palleteTableWidget, 1);
    connect(palleteTableWidget, SIGNAL(indexChanged(int)),
            mw, SLOT(setDrawingIndex(int)));
}

PalleteWindow::~PalleteWindow()
{
    delete palleteTableWidget;
    delete ui;
}

void PalleteWindow::closeEvent(QCloseEvent *event)
{
    emit closing();
    event->accept();
}
