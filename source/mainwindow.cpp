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
    loadWpe();
    loadMapping();
    loadRemapping();
    loadColorCycling();
    data->updateColorTable();
    connect(data, SIGNAL(colorTableChanged()),
            this, SLOT(updatePallete()));

    ui->grpImageScrollArea->setBackgroundRole(QPalette::Dark);
    connect(ui->act_open_grp, SIGNAL(triggered()), this, SLOT(loadGrp()));
    connect(ui->act_save_grp, SIGNAL(triggered()), this, SLOT(saveGrp()));
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
    openPallete();
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

void MainWindow::loadGrp()
{
#ifdef QT_DEBUG
    qDebug() << "SLOT MainWindow::loadGrp";
#endif
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
    qDebug() << "path" << path;

    data->getGrp()->save(path);
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
    ui->frameListWidget->addItem(QString::asprintf("Frame #%u", grp->getFrameCount()-1));
    ui->frameListWidget->setCurrentRow(grpFrameIndex+1);
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
    ui->frameListWidget->blockSignals(true);
    ui->frameListWidget->addItem(QString::asprintf("Frame #%u", grp->getFrameCount()-1));
    ui->frameListWidget->setCurrentRow(grpFrameIndex+1);
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
    ui->frameListWidget->blockSignals(true);
    delete ui->frameListWidget->takeItem(grp->getFrameCount());
    ui->frameListWidget->setCurrentRow((grpFrameIndex == 0)?0:grpFrameIndex-1);
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
    ui->frameListWidget->blockSignals(true);
    ui->frameListWidget->setCurrentRow(grpFrameIndex-1);
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
    ui->frameListWidget->blockSignals(true);
    ui->frameListWidget->setCurrentRow(grpFrameIndex+1);
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
    ui->frameListWidget->blockSignals(true);
    ui->frameListWidget->setCurrentRow(0); // @Think
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
    ui->frameListWidget->blockSignals(true);
    ui->frameListWidget->setCurrentRow(grp->getFrameCount()-1); // @Think
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
