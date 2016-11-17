#ifndef PALLETEWINDOW_H
#define PALLETEWINDOW_H

#include <QMainWindow>
#include <QCloseEvent>

namespace Ui {
class PalleteWindow;
}

class MainWindow;
class ComponentLoader;
class PalleteTableWidget;

class PalleteWindow : public QMainWindow
{
    Q_OBJECT

private:
    ComponentLoader *loader;
    PalleteTableWidget *palleteTableWidget;
    Ui::PalleteWindow *ui;

    void closeEvent(QCloseEvent *event);

public:
    explicit PalleteWindow(MainWindow *mw, QWidget *parent = 0);
    ~PalleteWindow();

signals:
    void closing();

};

#endif // PALLETEWINDOW_H
