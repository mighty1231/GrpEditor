#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QColor>
#include <QElapsedTimer>
#include <QPixmap>

namespace Ui {
class MainWindow;
}

class QLabel;
class QUndoStack;
class QListWidget;

class Grp;
class PalleteWindow;
class ComponentLoader;
class GrpConfigDialog;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    Grp * getGrp() {return grp;}
    void setGrp(Grp *grp) {this->grp=grp;}
    QString getGrpPath() {return grpPath;}
    void setGrpPath(QString qs) {grpPath = qs;}
    int getDrawingIndex() {return drawingIndex;}
    QRgb getOverflowedColor() {return overflowedColor;}
    ComponentLoader *getLoader() {return loader;}

    int getFrameIndex() {return grpFrameIndex;}
    void setFrameIndex(int i) {grpFrameIndex = i;}
    QListWidget *getFrameListWidget() {return frameListWidget;}

private:
    Grp *grp;
    QString grpPath;

    QImage *grpImage;
    QPixmap grpPixmap;
    ComponentLoader *loader;

    PalleteWindow *palleteWindow;
    Ui::MainWindow *ui;
    QListWidget *frameListWidget; // it could not be directly accessed

    // brush things
    int drawingIndex;
    QRgb overflowedColor;

    QElapsedTimer scalingTimer;
    int scaleFactor;
    int grpFrameIndex;

    GrpConfigDialog *grpConfigDialog;

    QLabel *statusBar_position;
    QLabel *statusBar_brushStatus;

    QUndoStack *undoStack;
    QAction *undoAction;
    QAction *redoAction;

    bool checkForUnsaved();

protected:
    bool eventFilter(QObject *obj, QEvent *event);

public slots:
    void newGrp();
    void loadGrp();
    void saveGrp();
    void saveAsGrp();

    void newGrp_ok(int, int);
    void newGrp_cancel();

    void openPallete();
    void palleteClosed();

    void frame_new();
    void frame_load();
    void frame_save();
    void frame_copy();
    void frame_delete();
    void frame_up();
    void frame_down();
    void frame_upmost();
    void frame_downmost();

    void frameScrolled(int);

    void updatePixel();
    void updatePallete();

    void setDrawingIndex(int i) {drawingIndex = i;}
    void setOverflowR(int i);
    void setOverflowG(int i);
    void setOverflowB(int i);
};

#endif // MAINWINDOW_H
