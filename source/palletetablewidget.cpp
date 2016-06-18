#include "palletetablewidget.h"
#include <QtGui>

PalleteTableWidget::PalleteTableWidget(QWidget *parent) : QFrame(parent)
{
    this->setFrameShape(QFrame::Panel);
    this->setFrameShadow(QFrame::Sunken);
    this->setLineWidth(3);

    data = Data::getInstance();
    memcpy(table, data->getColorTable().constData(), sizeof(table));

    connect(data, SIGNAL(colorTableChanged(QVector<QRgb>)),
            this, SLOT(updatePallete(QVector<QRgb>)));
}

QSize PalleteTableWidget::minimumSizeHint() const
{
    return QSize(MARGIN_HORI*2+INTERV_SPACE*15+ENTRY_MIN_WIDTH*16,
                 MARGIN_VERT*2+INTERV_SPACE*15+ENTRY_MIN_HEIGHT*16);
}

QSize PalleteTableWidget::sizeHint() const
{
    return QSize(400, 400);
}

void PalleteTableWidget::paintEvent(QPaintEvent *event)
{
    QFrame::paintEvent(event);

    int w = this->width();
    int h = this->height();

    int entry_w = (w-MARGIN_HORI*2-INTERV_SPACE*15)/16;
    int entry_h = (h-MARGIN_VERT*2-INTERV_SPACE*15)/16;

    QPainter painter(this);
    for (int i=0; i<16; i++) {
        for (int j=0; j<16; j++) {
            painter.fillRect(
                        MARGIN_HORI+(INTERV_SPACE+entry_w)*j,
                        MARGIN_VERT+(INTERV_SPACE+entry_h)*i,
                        entry_w,
                        entry_h,
                        QColor::fromRgb(table[i*16+j]));
        }
    }
}

void PalleteTableWidget::updatePallete(QVector<QRgb> rgb)
{
    memcpy(table, rgb.constData(), sizeof(table));
    update();
}
