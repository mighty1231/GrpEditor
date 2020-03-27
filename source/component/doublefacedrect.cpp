#include "doublefacedrect.h"

DoubleFacedRect::DoubleFacedRect(GrpFrame &frame)
    :oldface(frame), newface(frame.getHeight()), validRect(0, 0, 0, 0)
{
    this->width = frame.getWidth();
    this->height = frame.getHeight();

    for (int i = 0; i < height; i++) {
        newface[i] = new QByteArray(frame.scanLine(i), this->width);
    }
}

void DoubleFacedRect::drawPoint(int x, int y, char c)
{
    if (!validRect.contains(x, y)) {
        if (validRect.width() == 0) { /* empty */
            validRect.setRect(x, y, 1, 1);
        } else {
            validRect = validRect.united(QRect(x, y, 1, 1));
        }
    }
    (*newface[y])[x] = c;
}
