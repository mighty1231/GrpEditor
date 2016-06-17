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
    int t;
    PalleteWindow *palleteWindow;
    Data *data;
    Ui::MainWindow *ui;

public slots:
    void loadGrp();
    void saveGrp();

    void openPallete();
    void palleteClosed();
};

#endif // MAINWINDOW_H
