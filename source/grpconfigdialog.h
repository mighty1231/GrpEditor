#ifndef GRPCONFIGDIALOG_H
#define GRPCONFIGDIALOG_H

#include <QDialog>
#include <QCloseEvent>

namespace Ui {
class GrpConfigDialog;
}

class GrpConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GrpConfigDialog(QWidget *parent = 0);
    ~GrpConfigDialog();

private:
    Ui::GrpConfigDialog *ui;
    void closeEvent(QCloseEvent *event);

signals:
    void ok(int, int);
    void cancel();

public slots:
    void accepted();
    void rejected();
};

#endif // GRPCONFIGDIALOG_H
