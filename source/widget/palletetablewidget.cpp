#include "palletetablewidget.h"
#include <QPainter>
#include <QMouseEvent>
#include "component/loader.h"
#include "widget/mainwindow.h"

PalleteTableWidget::PalleteTableWidget(MainWindow *mw,
                                       ComponentLoader *cl, QWidget *parent)
    :QFrame(parent), mw(mw)
{
    loader = cl;

    this->setFrameShape(QFrame::Panel);
    this->setFrameShadow(QFrame::Sunken);
    this->setLineWidth(3);

    memcpy(table, loader->getColorTable().constData(), sizeof(table));

    connect(loader, SIGNAL(colorTableChanged()),
            this, SLOT(updatePallete()));
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

    entry_w = (w-MARGIN_HORI*2-INTERV_SPACE*15)/16;
    entry_h = (h-MARGIN_VERT*2-INTERV_SPACE*15)/16;

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

    int ind = mw->getDrawingIndex();
    QPen pen(QColor::fromRgb(255, 0, 0));
    pen.setWidth(2);
    painter.setPen(pen);
    painter.drawRect(
                MARGIN_HORI+(INTERV_SPACE+entry_w)*(ind%16)+1,
                MARGIN_VERT+(INTERV_SPACE+entry_h)*(ind>>4)+1,
                entry_w-1,
                entry_h-1);
}

void PalleteTableWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton) {
        event->accept();
        return;
    }
    int x = event->x();
    int y = event->y();

    if (x < MARGIN_HORI || y < MARGIN_VERT) {
        event->accept();
        return;
    }

    int i = (x-MARGIN_HORI)/(INTERV_SPACE+entry_w);
    int j = (y-MARGIN_VERT)/(INTERV_SPACE+entry_h);
    if (i<16 && j<16
            && (x-MARGIN_HORI-i*(INTERV_SPACE+entry_w)) < entry_w
            && (y-MARGIN_HORI-j*(INTERV_SPACE+entry_h)) < entry_h) {
        qDebug("x %d y %d i %d j %d res %d", x, y, i, j, j*16+i);
        emit indexChanged(j*16+i);
        update();
    }
    event->accept();
}

void PalleteTableWidget::updatePallete()
{
#ifdef QT_DEBUG
    qDebug() << "SLOT PalleteTableWidget::updatePallete";
#endif
    memcpy(table, loader->getColorTable().constData(), sizeof(table));
    update();
}
