#ifndef PALLETEWINDOW_H
#define PALLETEWINDOW_H

#include <QMainWindow>

namespace Ui {
class PalleteWindow;
}

class PalleteWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit PalleteWindow(QWidget *parent = 0);
    ~PalleteWindow();



private:
    Ui::PalleteWindow *ui;
};

#endif // PALLETEWINDOW_H
