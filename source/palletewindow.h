#ifndef PALLETEWINDOW_H
#define PALLETEWINDOW_H

#include <QMainWindow>
#include <QCloseEvent>
#include "data.h"
#include "palletetablewidget.h"
#include "mainwindow.h"

namespace Ui {
class PalleteWindow;
}

class MainWindow;
class PalleteWindow : public QMainWindow
{
    Q_OBJECT

private:
    MainWindow *mainWindow;
    Data *data;
    PalleteTableWidget *palleteTableWidget;
    Ui::PalleteWindow *ui;

    void closeEvent(QCloseEvent *event);

public:
    explicit PalleteWindow(QWidget *parent = 0);
    ~PalleteWindow();

signals:
    void closing();

};

#endif // PALLETEWINDOW_H
