#ifndef PALLETETABLEWIDGET_H
#define PALLETETABLEWIDGET_H

#include <QFrame>
#include "data.h"

class PalleteTableWidget : public QFrame
{
    Q_OBJECT
private:
    static const int MARGIN_HORI = 10;
    static const int MARGIN_VERT = 8;
    static const int INTERV_SPACE = 4;

    static const int ENTRY_MIN_WIDTH = 10;
    static const int ENTRY_MIN_HEIGHT = 10;


    Data *data;
    QRgb table[256];
    int entry_w;
    int entry_h;
public:
    explicit PalleteTableWidget(QWidget *parent = 0);

    QSize minimumSizeHint() const Q_DECL_OVERRIDE;
    QSize sizeHint() const Q_DECL_OVERRIDE;

protected:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *event);

signals:
    void indexChanged(int);

public slots:
    void updatePallete();
};

#endif // PALLETETABLEWIDGET_H
