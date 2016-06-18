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
    static const int INTERV_SPACE = 5;

    static const int ENTRY_MIN_WIDTH = 10;
    static const int ENTRY_MIN_HEIGHT = 10;
    Data *data;
    QRgb table[256];
public:
    explicit PalleteTableWidget(QWidget *parent = 0);

    QSize minimumSizeHint() const Q_DECL_OVERRIDE;
    QSize sizeHint() const Q_DECL_OVERRIDE;

protected:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;

signals:

public slots:
    void updatePallete(QVector<QRgb> rgb);
};

#endif // PALLETETABLEWIDGET_H
