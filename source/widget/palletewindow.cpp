#include "palletewindow.h"
#include "ui_palletewindow.h"

PalleteWindow::PalleteWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::PalleteWindow)
{
    ui->setupUi(this);

    data = Data::getInstance();
    QVector<Wpe *> wpes = data->getWpes();
    QVector<Mapping *> mappings = data->getMappings();
    QVector<Remapping *> remappings = data->getRemappings();
    QVector<ColorCycling *> colorCyclings = data->getColorCyclings();

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

    ui->combo_wpe->setCurrentIndex(data->getWpeIndex());
    ui->combo_map->setCurrentIndex(data->getMappingIndex());
    ui->combo_rem->setCurrentIndex(data->getRemappingIndex());
    ui->combo_cc->setCurrentIndex(data->getColorCyclingIndex());

    connect(ui->combo_wpe, SIGNAL(currentIndexChanged(int)),
            data, SLOT(setWpeIndex(int)));
    connect(ui->combo_map, SIGNAL(currentIndexChanged(int)),
            data, SLOT(setMappingIndex(int)));
    connect(ui->combo_rem, SIGNAL(currentIndexChanged(int)),
            data, SLOT(setRemappingIndex(int)));
    connect(ui->combo_cc, SIGNAL(currentIndexChanged(int)),
            data, SLOT(setColorCyclingIndex(int)));


    QRgb o = data->getOverflowedColor();
    ui->overflow_r->setValue(qRed(o));
    ui->overflow_g->setValue(qGreen(o));
    ui->overflow_b->setValue(qBlue(o));
    connect(ui->overflow_r, SIGNAL(valueChanged(int)),
            data, SLOT(setOverflowR(int)));
    connect(ui->overflow_g, SIGNAL(valueChanged(int)),
            data, SLOT(setOverflowG(int)));
    connect(ui->overflow_b, SIGNAL(valueChanged(int)),
            data, SLOT(setOverflowB(int)));

    palleteTableWidget = new PalleteTableWidget(this);
    ui->layout0->addWidget(palleteTableWidget, 1);
//    ui->palleteLabel->setPixmap();
    connect(palleteTableWidget, SIGNAL(indexChanged(int)),
            data, SLOT(setDrawingIndex(int)));
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
