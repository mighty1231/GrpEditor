#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QScrollBar>
#include <QUndoStack>
#include <QEvent>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QMessageBox>
#include <QFileDialog>

#include "component/loader.h"
#include "component/grp.h"
#include "component/grpframe.h"
#include "component/remapping.h"
#include "widget/grpconfigdialog.h"
#include "widget/palletewindow.h"
#include "command/createframecommand.h"
#include "command/deleteframecommand.h"
#include "command/swapframecommand.h"
#include "command/dragframecommand.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),
    ui(new Ui::MainWindow), scaleFactor(1), grpImage(NULL)
{
    ui->setupUi(this);

    grp = NULL;
    drawingIndex = 0;
    overflowedColor = qRgb(255, 0, 255);

    /* load component files */
    loader = new ComponentLoader(this);
    connect(loader, SIGNAL(colorTableChanged()),
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

    // Undo framework
    undoStack = new QUndoStack(this);
    undoAction = undoStack->createUndoAction(this, tr("&Undo"));
    undoAction->setShortcut(QKeySequence::Undo);
    redoAction = undoStack->createRedoAction(this, tr("&Undo"));
    redoAction->setShortcut(QKeySequence::Redo);
    ui->menuEdit->addAction(undoAction);
    ui->menuEdit->addAction(redoAction);

    // ui
    statusBar_position = new QLabel();
    statusBar_brushStatus = new QLabel();
    ui->statusBar->addPermanentWidget(statusBar_position, 1);
    ui->statusBar->addPermanentWidget(statusBar_brushStatus, 2);

    show();
    palleteWindow = NULL;
    grpConfigDialog = NULL;
    frameListWidget = ui->frameListWidget;
    openPallete();
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::Wheel && grp) {
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
               && grp){
        QMouseEvent *mEvent = static_cast<QMouseEvent *>(event);

        int x = mEvent->x() / scaleFactor;
        int y = mEvent->y() / scaleFactor;

        if (x < 0 || x >= grp->getWidth()
                || y < 0 || y >= grp->getHeight()) {
            statusBar_position->clear();
            return true;
        }

        GrpFrame *frame = grp->getFrame(grpFrameIndex);

        quint8 color = (quint8) frame->at(x, y);
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
    if (grp != NULL) {
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
    grp = Grp::loadEmpty(width, height);
    grpPath.clear();

    ui->frameListWidget->blockSignals(true);
    ui->frameListWidget->clear();
    ui->frameListWidget->addItem(QString::asprintf("Frame #%u", 0));
    ui->frameListWidget->setCurrentRow(0);
    ui->frameListWidget->blockSignals(false);
    grpFrameIndex = 0;
    scaleFactor = 1;

    updatePixel();
    grpConfigDialog = NULL;

    // Undo framework
    undoStack->clear();
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

    ui->frameListWidget->blockSignals(true);
    if (grp != NULL) {
        delete grp;
        ui->frameListWidget->clear();
    }
    for (int i=0; i<new_grp->getFrameCount(); i++) {
        ui->frameListWidget->addItem(new_grp->getFrame(i)->getName());
    }
    grp = new_grp;
    grpPath = fname;
    ui->frameListWidget->setCurrentRow(0);
    ui->frameListWidget->blockSignals(false);

    grpFrameIndex = 0;
    scaleFactor = 1;
    updatePixel();

    // Undo framework
    undoStack->clear();
}

void MainWindow::saveGrp()
{
#ifdef QT_DEBUG
    qDebug() << "SLOT MainWindow::saveGrp";
#endif

    if (grpPath.isEmpty()) {
        saveAsGrp();
    } else {
        qDebug() << "path" << grpPath;

        grp->save(grpPath);
    }
}

void MainWindow::saveAsGrp()
{
#ifdef QT_DEBUG
    qDebug() << "SLOT MainWindow::saveAsGrp";
#endif

    grpPath = QFileDialog::getSaveFileName(
                this,
                tr("Save As"),
                tr(""),
                tr("Grp files (*.grp)"));
    grp->save(grpPath);
}

void MainWindow::openPallete()
{
#ifdef QT_DEBUG
    qDebug() << "SLOT MainWindow::openPallete";
#endif

    if (palleteWindow == NULL) {
        palleteWindow = new PalleteWindow(this, this);
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
    if (grpFrameIndex != index && grp) {
        grpFrameIndex = index;
        updatePixel();
    }
}

void MainWindow::updatePixel()
{
#ifdef QT_DEBUG
    qDebug() << "SLOT MainWindow::updatePixel";
#endif
    int width = grp->getWidth();
    int height = grp->getHeight();
    GrpFrame *frame = grp->getFrame(grpFrameIndex);

    int vertScrollValue = 0;
    int horiScrollValue = 0;

    if (grpImage == NULL) {
        grpImage = new QImage(width, height, QImage::Format_Indexed8);
        grpImage->setColorTable(loader->getColorTable());
    } else if ((grpImage->width() != width)
               || (grpImage->height() != height)) {
        delete grpImage;
        grpImage = new QImage(width, height, QImage::Format_Indexed8);
        grpImage->setColorTable(loader->getColorTable());
    } else {
        vertScrollValue = ui->grpImageScrollArea->verticalScrollBar()->value();
        horiScrollValue = ui->grpImageScrollArea->horizontalScrollBar()->value();
    }

    for (int i=0; i<height; i++) {
        memcpy(grpImage->scanLine(i), frame->scanLine(i), width);
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
    grpImage->setColorTable(loader->getColorTable());
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
    if (grp == NULL)
        return;

    GrpFrame blankFrame("blank", grp->getWidth(), grp->getHeight());
    QUndoCommand *cmd = new CreateFrameCommand(this,
                                               grpFrameIndex+1,
                                               blankFrame);
    undoStack->push(cmd);
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
    if (grp == NULL)
        return;

    GrpFrame copiedFrame(*grp->getFrame(grpFrameIndex));
    QUndoCommand *cmd = new CreateFrameCommand(this,
                                               grpFrameIndex+1,
                                               copiedFrame);
    undoStack->push(cmd);
}

void MainWindow::frame_delete()
{
#ifdef QT_DEBUG
    qDebug() << "SLOT MainWindow::frame_delete";
#endif
    if (grp == NULL)
        return;

    QUndoCommand *cmd = new DeleteFrameCommand(this,
                                               grpFrameIndex);
    undoStack->push(cmd);
}

void MainWindow::frame_up()
{
#ifdef QT_DEBUG
    qDebug() << "SLOT MainWindow::frame_up";
#endif
    if (grp == NULL || grpFrameIndex == 0)
        return;

    QUndoCommand *cmd = new SwapFrameCommand(this,
                                             grpFrameIndex,
                                             grpFrameIndex-1);
    undoStack->push(cmd);
}

void MainWindow::frame_down()
{
#ifdef QT_DEBUG
    qDebug() << "SLOT MainWindow::frame_down";
#endif
    if (grp == NULL || grpFrameIndex == grp->getFrameCount()-1)
        return;

    QUndoCommand *cmd = new SwapFrameCommand(this,
                                             grpFrameIndex,
                                             grpFrameIndex+1);
    undoStack->push(cmd);
}

void MainWindow::frame_upmost()
{
#ifdef QT_DEBUG
    qDebug() << "SLOT MainWindow::frame_upmost";
#endif
    if (grp == NULL || grpFrameIndex == 0)
        return;

    QUndoCommand *cmd = new DragFrameCommand(this,
                                             grpFrameIndex,
                                             0);
    undoStack->push(cmd);

//    grp->upmostFrame(grpFrameIndex);
//    grpFrameIndex = 0; // @Think
//    ui->frameListWidget->blockSignals(true);
//    ui->frameListWidget->setCurrentRow(grpFrameIndex);
//    ui->frameListWidget->blockSignals(false);
}

void MainWindow::frame_downmost()
{
#ifdef QT_DEBUG
    qDebug() << "SLOT MainWindow::frame_downmost";
#endif
    if (grp == NULL || grpFrameIndex == grp->getFrameCount()-1)
        return;

    QUndoCommand *cmd = new DragFrameCommand(this,
                                             grpFrameIndex,
                                             grp->getFrameCount()-1);
    undoStack->push(cmd);
//    grp->downmostFrame(grpFrameIndex);
//    grpFrameIndex = grp->getFrameCount()-1; // @Think
//    ui->frameListWidget->blockSignals(true);
//    ui->frameListWidget->setCurrentRow(grpFrameIndex);
//    ui->frameListWidget->blockSignals(false);
}

void MainWindow::setOverflowR (int i) {
#ifdef QT_DEBUG
    qDebug() << "SLOT ComponentLoader::setOverflowR";
#endif
    int r, g, b;
    r = i;
    g = qGreen(overflowedColor);
    b = qBlue(overflowedColor);
    overflowedColor = qRgb(r, g, b);

    if (loader->getRemappings()[loader->getRemappingIndex()]->getSize() != 256)
        loader->updateColorTable();
}

void MainWindow::setOverflowG (int i) {
#ifdef QT_DEBUG
    qDebug() << "SLOT ComponentLoader::setOverflowG";
#endif
    int r, g, b;
    r = qRed(overflowedColor);
    g = i;
    b = qBlue(overflowedColor);
    overflowedColor = qRgb(r, g, b);

    if (loader->getRemappings()[loader->getRemappingIndex()]->getSize() != 256)
        loader->updateColorTable();
}

void MainWindow::setOverflowB (int i) {
#ifdef QT_DEBUG
    qDebug() << "SLOT ComponentLoader::setOverflowB";
#endif
    int r, g, b;
    r = qRed(overflowedColor);
    g = qGreen(overflowedColor);
    b = i;
    overflowedColor = qRgb(r, g, b);

    if (loader->getRemappings()[loader->getRemappingIndex()]->getSize() != 256)
        loader->updateColorTable();
}

MainWindow::~MainWindow()
{
    delete ui;
    if (palleteWindow != NULL)
        delete palleteWindow;

    if (grpImage != NULL)
        delete grpImage;
}
