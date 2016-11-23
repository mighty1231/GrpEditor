#ifndef DOUBLEFACEDRECT_H
#define DOUBLEFACEDRECT_H

#include <QRect>
#include <QVector>
#include <QByteArray>
#include "component/grpframe.h"

/* help drawing over frame */
class DoubleFacedRect
{
public:
    DoubleFacedRect(GrpFrame&);

    void drawPoint(int, int, char);

private:
    int width, height;

    GrpFrame oldface;
    QVector<QByteArray *> newface;
    QRect validRect;
};

#endif // DOUBLEFACEDRECT_H
