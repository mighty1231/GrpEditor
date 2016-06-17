#include "colorcycling.h"
#include "globals.h"

ColorCycling::ColorCycling()
{
    for (int i=0; i<256; i++) {
        map[i] = i;
    }
}

ColorCycling * ColorCycling::load(QString name, QString fname)
{
    QFile file(fname);

    if (!file.open(QIODevice::ReadOnly)) {
        return NULL;
    }

    QByteArray ba = file.readAll();
    file.close();

    if (ba.length() != 8*16) {
        return NULL;
    }

    ColorCycling *colorcycling = new ColorCycling();
    memcpy(colorcycling->entries, ba.data(), 8*16);
    for (int i=0; i<8; i++) {
        if (colorcycling->entries[i].timer == 0)
            break;
        if (colorcycling->entries[i].working == 1
                && colorcycling->entries[i].__unknown8 != 0) {
            delete colorcycling;
            return NULL;
        }
    }
    colorcycling->name = name;
    return colorcycling;
}

ColorCycling * ColorCycling::loadDefault()
{
    ColorCycling *colorcycling = new ColorCycling();
    memset(colorcycling->entries, 0, 8*16);
    colorcycling->name = DEFAULT_OBJNAME;
    return colorcycling;
}

QString ColorCycling::getName()
{
    return name;
}
