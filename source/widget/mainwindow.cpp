#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtCore>
#include <QtWidgets>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),
    ui(new Ui::MainWindow), scaleFactor(1), grpImage(NULL)
{
    ui->setupUi(this);

    /* load data files */
    data = Data::getInstance();
    data->updateColorTable();
    connect(data, SIGNAL(colorTableChanged()),
            this, SLOT(updatePallete()));

    ui->grpImageScrollArea->setBackgroundRole(QPalette::Dark);
    connect(ui->act_new_grp, SIGNAL(triggered()), this, SLOT(newGrp()));
    connect(ui->act_open_grp, SIGNAL(triggered()), this, SLOT(loadGrp()));
    connect(ui->act_save_grp, SIGNAL(triggered()), this, SLOT(saveGrp()));
    connect(ui->act_save_as_grp, SIGNAL(triggered()), this, SLOT(saveAsGrp()));
    connect(ui->act_pallete, SIGNAL(toggled(bool)), this, SLOT(openPallete()));

    connect(ui->frameListWidget, SIGNAL(currentRowChanged(int)),
            this, SLOT(frameScrolled(int)));

    connect(ui->newFrameButton, SIGNAL(clicked(bool)),
            this, SLOT(frame_new()));
    connect(ui->loadFrameButton, SIGNAL(clicked(bool)),
            this, SLOT(frame_load()));
    connect(ui->saveFrameButton, SIGNAL(clicked(bool)),
            this, SLOT(frame_save()));
    connect(ui->copyFrameButton, SIGNAL(clicked(bool)),
            this, SLOT(frame_copy()));
    connect(ui->deleteFrameButton, SIGNAL(clicked(bool)),
            this, SLOT(frame_delete()));
    connect(ui->upFrameButton, SIGNAL(clicked(bool)),
            this, SLOT(frame_up()));
    connect(ui->downFrameButton, SIGNAL(clicked(bool)),
            this, SLOT(frame_down()));
    connect(ui->upmostFrameButton, SIGNAL(clicked(bool)),
            this, SLOT(frame_upmost()));
    connect(ui->downmostFrameButton, SIGNAL(clicked(bool)),
            this, SLOT(frame_downmost()));

    scalingTimer.start();

    installEventFilter(this);
    ui->grpImageScrollArea->verticalScrollBar()->installEventFilter(this);

    ui->grpLabel->setMouseTracking(true);
    ui->grpLabel->installEventFilter(this);

    statusBar_position = new QLabel();
    statusBar_brushStatus = new QLabel();
    ui->statusBar->addPermanentWidget(statusBar_position, 1);
    ui->statusBar->addPermanentWidget(statusBar_brushStatus, 2);

    show();
    palleteWindow = NULL;
    grpConfigDialog = NULL;
    openPallete();
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::Wheel && data->getGrp()) {
        if (obj == ui->grpImageScrollArea->verticalScrollBar())
            return true;
        if (obj != this || !scalingTimer.hasExpired(40))
            return QObject::eventFilter(obj, event);

        qDebug() << obj << event;
        scalingTimer.restart();

        int delta = static_cast<QWheelEvent *>(event)->delta();
        if (delta > 0) {
            scaleFactor++;
        } else if (delta < 0 && scaleFactor > 1){
            scaleFactor--;
        }
        ui->grpLabel->setPixmap(
                    grpPixmap.scaled(
                        grpImage->width()*scaleFactor,
                        grpImage->height()*scaleFactor,
                        Qt::AspectRatioMode::IgnoreAspectRatio,
                        Qt::FastTransformation)
                    );
        return true;
    } else if (obj == ui->grpLabel
               && event->type() == QEvent::MouseMove
               && data->getGrp()){
        QMouseEvent *mEvent = static_cast<QMouseEvent *>(event);

        int x = mEvent->x() / scaleFactor;
        int y = mEvent->y() / scaleFactor;

        Grp *grp = data->getGrp();

        if (x < 0 || x >= grp->getWidth()
                || y < 0 || y >= grp->getHeight()) {
            statusBar_position->clear();
            return true;
        }

        QByteArray *frame = grp->getFrame(grpFrameIndex);

        quint8 color = (quint8) frame->at(grp->getWidth()*y+x);
        statusBar_position->setText(
                    QString::asprintf("(%d, %d), color #%d",
                                     x, y, color));
        return true;
    } else {
        return QObject::eventFilter(obj, event);
    }
}

bool MainWindow::checkForUnsaved()
{
    /* if grp is already loaded, warning message box */
    if (data->getGrp() != NULL) {
        /* @TODO Warning for only grp is modified by user */
        QMessageBox::StandardButton rep;
        rep = QMessageBox::warning(this, "Warning", "If you had modified grp, you had to save it. Did you check?",
                                   QMessageBox::Yes|QMessageBox::No);
        return (rep == QMessageBox::Yes);
    }

    return true;
}

void MainWindow::newGrp()
{
    if (grpConfigDialog != NULL) {
        /* @TODO focus original grpConfigDialog */
        QMessageBox::warning(this, "New Grp",
                             "You already have window for it!",
                             QMessageBox::Ok);
        return;
    }

    if (!checkForUnsaved())
        return;

    grpConfigDialog = new GrpConfigDialog(this);

    connect(grpConfigDialog, SIGNAL(ok(int, int)),
            this, SLOT(newGrp_ok(int, int)));
    connect(grpConfigDialog, SIGNAL(cancel()),
            this, SLOT(newGrp_cancel()));
    grpConfigDialog->show();
}

void MainWindow::newGrp_ok(int width, int height)
{
    data->setGrp(Grp::loadEmpty(width, height));
    data->setGrpPath(QString());
    ui->frameListWidget->blockSignals(true);
    ui->frameListWidget->clear();
    ui->frameListWidget->addItem(QString::asprintf("Frame #%u", 0));
    ui->frameListWidget->setCurrentRow(0);
    ui->frameListWidget->blockSignals(false);
    grpFrameIndex = 0;
    scaleFactor = 1;

    updatePixel();
    grpConfigDialog = NULL;
}

void MainWindow::newGrp_cancel()
{
    grpConfigDialog = NULL;
}

void MainWindow::loadGrp()
{
#ifdef QT_DEBUG
    qDebug() << "SLOT MainWindow::loadGrp";
#endif
    if (!checkForUnsaved())
        return;

    QString fname = QFileDialog::getOpenFileName(
                this,
                tr("Open File"),
                tr(""),
                tr("Grp files (*.grp)"));

    if (fname.isEmpty()) return;
    qDebug() << fname;

    Grp *new_grp = Grp::load(fname);
    if (new_grp == NULL) {
        QMessageBox::critical(this, "Error", "Error happened in grp loading");
        return;
    }

    Grp *old_grp = data->getGrp();
    ui->frameListWidget->blockSignals(true);
    if (old_grp != NULL) {
        delete old_grp;
        ui->frameListWidget->clear();
    }
    for (int i=0; i<new_grp->getFrameCount(); i++) {
        ui->frameListWidget->addItem(QString::asprintf("Frame #%u", i));
    }
    data->setGrp(new_grp);
    data->setGrpPath(fname);
    ui->frameListWidget->setCurrentRow(0);
    ui->frameListWidget->blockSignals(false);

    grpFrameIndex = 0;
    scaleFactor = 1;
    updatePixel();
}

void MainWindow::saveGrp()
{
#ifdef QT_DEBUG
    qDebug() << "SLOT MainWindow::saveGrp";
#endif

    QString path = data->getGrpPath();
    if (path.isEmpty()) {
        saveAsGrp();
    } else {
        qDebug() << "path" << path;

        data->getGrp()->save(path);
    }
}

void MainWindow::saveAsGrp()
{
#ifdef QT_DEBUG
    qDebug() << "SLOT MainWindow::saveAsGrp";
#endif

    QString fname = QFileDialog::getSaveFileName(
                this,
                tr("Save As"),
                tr(""),
                tr("Grp files (*.grp)"));
    data->setGrpPath(fname);
    data->getGrp()->save(fname);
}

void MainWindow::openPallete()
{
#ifdef QT_DEBUG
    qDebug() << "SLOT MainWindow::openPallete";
#endif

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
#ifdef QT_DEBUG
    qDebug() << "SLOT MainWindow::palleteClosed";
#endif
    palleteWindow = NULL;
}

void MainWindow::frameScrolled(int index)
{
    if (grpFrameIndex != index && data->getGrp()) {
        grpFrameIndex = index;
        updatePixel();
    }
}

void MainWindow::updatePixel()
{
#ifdef QT_DEBUG
    qDebug() << "SLOT MainWindow::updatePixel";
#endif
    Grp *grp = data->getGrp();
    int width = grp->getWidth();
    int height = grp->getHeight();
    char *frame = grp->getFrame(grpFrameIndex)->data();

    int vertScrollValue = 0;
    int horiScrollValue = 0;

    if (grpImage == NULL) {
        grpImage = new QImage(width, height, QImage::Format_Indexed8);
        grpImage->setColorTable(data->getColorTable());
    } else if ((grpImage->width() != width)
               || (grpImage->height() != height)) {
        delete grpImage;
        grpImage = new QImage(width, height, QImage::Format_Indexed8);
        grpImage->setColorTable(data->getColorTable());
    } else {
        vertScrollValue = ui->grpImageScrollArea->verticalScrollBar()->value();
        horiScrollValue = ui->grpImageScrollArea->horizontalScrollBar()->value();
    }

    for (int i=0; i<height; i++) {
        memcpy(grpImage->scanLine(i), frame+i*width, width);
    }
    grpPixmap.convertFromImage(*grpImage);
    ui->grpLabel->setPixmap(grpPixmap.scaled(
                                width*scaleFactor,
                                height*scaleFactor,
                                Qt::AspectRatioMode::IgnoreAspectRatio,
                                Qt::FastTransformation));

    ui->grpImageScrollArea->verticalScrollBar()->setValue(vertScrollValue);
    ui->grpImageScrollArea->horizontalScrollBar()->setValue(horiScrollValue);
}

void MainWindow::updatePallete()
{
#ifdef QT_DEBUG
    qDebug() << "SLOT MainWindow::updatePallete";
#endif
    if (grpImage == NULL)
        return;
    grpImage->setColorTable(data->getColorTable());
    grpPixmap.convertFromImage(*grpImage);
    ui->grpLabel->setPixmap(grpPixmap.scaled(
                                grpImage->width()*scaleFactor,
                                grpImage->height()*scaleFactor,
                                Qt::AspectRatioMode::IgnoreAspectRatio,
                                Qt::FastTransformation));
}

void MainWindow::frame_new()
{
#ifdef QT_DEBUG
    qDebug() << "SLOT MainWindow::frame_new";
#endif
    Grp *grp = data->getGrp();
    if (grp == NULL)
        return;

    grp->insertFrame(grpFrameIndex+1);
    grpFrameIndex++;
    ui->frameListWidget->addItem(QString::asprintf("Frame #%u", grp->getFrameCount()-1));
    ui->frameListWidget->blockSignals(true);
    ui->frameListWidget->setCurrentRow(grpFrameIndex);
    ui->frameListWidget->blockSignals(false);

    updatePixel();
}

void MainWindow::frame_load()
{
#ifdef QT_DEBUG
    qDebug() << "SLOT MainWindow::frame_load";
#endif
}

void MainWindow::frame_save()
{
#ifdef QT_DEBUG
    qDebug() << "SLOT MainWindow::frame_save";
#endif
}

void MainWindow::frame_copy()
{
#ifdef QT_DEBUG
    qDebug() << "SLOT MainWindow::frame_copy";
#endif
    Grp *grp = data->getGrp();
    if (grp == NULL)
        return;

    grp->copyFrame(grpFrameIndex);
    grpFrameIndex++;
    ui->frameListWidget->blockSignals(true);
    ui->frameListWidget->addItem(QString::asprintf("Frame #%u", grp->getFrameCount()-1));
    ui->frameListWidget->setCurrentRow(grpFrameIndex);
    ui->frameListWidget->blockSignals(false);
}

void MainWindow::frame_delete()
{
#ifdef QT_DEBUG
    qDebug() << "SLOT MainWindow::frame_delete";
#endif

    Grp *grp = data->getGrp();
    if (grp == NULL)
        return;

    if (grp->getFrameCount() == 1)
        return;
    grp->deleteFrame(grpFrameIndex);
    grpFrameIndex = (grpFrameIndex == 0)?0:grpFrameIndex-1;
    ui->frameListWidget->blockSignals(true);
    delete ui->frameListWidget->takeItem(grp->getFrameCount());
    ui->frameListWidget->setCurrentRow(grpFrameIndex);
    ui->frameListWidget->blockSignals(false);

    updatePixel();
}

void MainWindow::frame_up()
{
#ifdef QT_DEBUG
    qDebug() << "SLOT MainWindow::frame_up";
#endif
    Grp *grp = data->getGrp();
    if (grp == NULL)
        return;

    if (grpFrameIndex == 0)
        return;
    grp->swapFrame(grpFrameIndex, grpFrameIndex-1);
    grpFrameIndex--;
    ui->frameListWidget->blockSignals(true);
    ui->frameListWidget->setCurrentRow(grpFrameIndex);
    ui->frameListWidget->blockSignals(false);
}

void MainWindow::frame_down()
{
#ifdef QT_DEBUG
    qDebug() << "SLOT MainWindow::frame_down";
#endif
    Grp *grp = data->getGrp();
    if (grp == NULL)
        return;

    if (grpFrameIndex == grp->getFrameCount()-1)
        return;
    grp->swapFrame(grpFrameIndex, grpFrameIndex+1);
    grpFrameIndex++;
    ui->frameListWidget->blockSignals(true);
    ui->frameListWidget->setCurrentRow(grpFrameIndex);
    ui->frameListWidget->blockSignals(false);
}

void MainWindow::frame_upmost()
{
#ifdef QT_DEBUG
    qDebug() << "SLOT MainWindow::frame_upmost";
#endif
    Grp *grp = data->getGrp();
    if (grp == NULL)
        return;

    if (grpFrameIndex == 0)
        return;
    grp->upmostFrame(grpFrameIndex);
    grpFrameIndex = 0; // @Think
    ui->frameListWidget->blockSignals(true);
    ui->frameListWidget->setCurrentRow(grpFrameIndex);
    ui->frameListWidget->blockSignals(false);
}

void MainWindow::frame_downmost()
{
#ifdef QT_DEBUG
    qDebug() << "SLOT MainWindow::frame_downmost";
#endif
    Grp *grp = data->getGrp();
    if (grp == NULL)
        return;

    if (grpFrameIndex == grp->getFrameCount()-1)
        return;
    grp->downmostFrame(grpFrameIndex);
    grpFrameIndex = grp->getFrameCount()-1; // @Think
    ui->frameListWidget->blockSignals(true);
    ui->frameListWidget->setCurrentRow(grpFrameIndex);
    ui->frameListWidget->blockSignals(false);
}


MainWindow::~MainWindow()
{
    delete ui;
    if (palleteWindow != NULL)
        delete palleteWindow;

    if (grpImage != NULL)
        delete grpImage;
}
