#ifndef COLORCYCLING_H
#define COLORCYCLING_H

#include <QtCore>

class ColorCycling
{
private:
    struct CycleEntry {
        char working;
        unsigned char period;
        unsigned char timer;
        unsigned char low;
        char __unknown4;
        unsigned char high;
        char __unknown6[2];
        int __unknown8;
        char __unknownC[4];
    };

    CycleEntry entries[8];
    char map[256];
    QString name;

    ColorCycling();
public:
    static ColorCycling * load(QString name, QString fname);
    static ColorCycling * loadDefault();

    QString getName();
    char *getMap();

    bool processTick();
    quint8 getIndex(int index);
};

#endif // COLORCYCLING_H
