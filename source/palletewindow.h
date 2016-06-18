#ifndef PALLETEWINDOW_H
#define PALLETEWINDOW_H

#include <QMainWindow>
#include <QCloseEvent>
#include "data.h"
#include "palletetablewidget.h"

namespace Ui {
class PalleteWindow;
}

class PalleteWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit PalleteWindow(QWidget *parent = 0);
    ~PalleteWindow();

signals:
    void closing();

private:
    Data *data;
    PalleteTableWidget *palleteTableWidget;
    Ui::PalleteWindow *ui;

    void closeEvent(QCloseEvent *event);
};

#endif // PALLETEWINDOW_H
