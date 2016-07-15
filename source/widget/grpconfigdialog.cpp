#include "grpconfigdialog.h"
#include "ui_grpconfigdialog.h"
#include <QDebug>

GrpConfigDialog::GrpConfigDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GrpConfigDialog)
{
    ui->setupUi(this);

    connect(ui->buttonBox, SIGNAL(accepted()),
            this, SLOT(accepted()));
    connect(ui->buttonBox, SIGNAL(rejected()),
            this, SLOT(rejected()));
}

void GrpConfigDialog::closeEvent(QCloseEvent *event)
{
    emit cancel();
    event->accept();
}

void GrpConfigDialog::accepted()
{
#ifdef QT_DEBUG
    qDebug() << "GrpConfigDialog::accepted";
#endif

    emit ok(ui->spinBox_w->value(), ui->spinBox_h->value());
}

void GrpConfigDialog::rejected()
{
#ifdef QT_DEBUG
    qDebug() << "GrpConfigDialog::rejected";
#endif

    emit cancel();
}

GrpConfigDialog::~GrpConfigDialog()
{
    delete ui;
}
