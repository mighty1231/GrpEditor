#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "palletewindow.h"
#include "data.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void loadData();
    void loadWpe();
    void loadMapping();
    void loadRemapping();
    void loadColorCycling();

private:
    QImage *grpImage;
    QPixmap grpPixmap;
    Data *data;

    PalleteWindow *palleteWindow;
    Ui::MainWindow *ui;

public slots:
    void loadGrp();
    void saveGrp();

    void openPallete();
    void palleteClosed();

    void updatePixelData(int, int, char *);
    void updatePallete(QVector<QRgb>);
};

#endif // MAINWINDOW_H
