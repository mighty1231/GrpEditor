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
        if (colorcycling->entries[i].period == 0)
            break;
        if (colorcycling->entries[i].working != 0
                && (colorcycling->entries[i].__unknown8 != 0
                    || colorcycling->entries[i].low
                    >= colorcycling->entries[i].high)) {
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

bool ColorCycling::processTick()
{
    bool res = false;
    CycleEntry *cur;
    for (int i=0; i<8; i++) {
        cur = &(entries[i]);
        if (cur->period == 0)
            break;
        if (cur->working != 0 && --(cur->timer) == 0) {
            res = true;
            cur->timer = cur->period;
            char tmp = map[cur->high];
            memcpy(map+cur->low+1, map+cur->low, cur->high-cur->low);
            map[cur->low] = tmp;
        }
    }
    return res;
}

quint8 ColorCycling::getIndex(int index)
{
    Q_ASSERT(index >= 0 && index < 256);
    return map[index];
}


