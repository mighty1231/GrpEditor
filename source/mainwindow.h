#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include "palletewindow.h"
#include "data.h"
#include "grpconfigdialog.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    QImage *grpImage;
    QPixmap grpPixmap;
    Data *data;

    PalleteWindow *palleteWindow;
    Ui::MainWindow *ui;

    QElapsedTimer scalingTimer;
    int scaleFactor;
    int grpFrameIndex;

    GrpConfigDialog *grpConfigDialog;

    QLabel *statusBar_position;
    QLabel *statusBar_brushStatus;

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
};

#endif // MAINWINDOW_H
