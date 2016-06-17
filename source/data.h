#ifndef DATA_H
#define DATA_H

#include "wpe.h"
#include "mapping.h"
#include "remapping.h"
#include "colorcycling.h"
#include "grp.h"


class Data
{
private:
    static Data * instance;

    /* objects
     * and index of current selected one of them */
    Grp *grp;
    int icGrp; // frame index

    QVector<Wpe *> wpes;
    int icWpe;

    QVector<Mapping *> mappings;
    int icMapping;

    QVector<Remapping *> remappings;
    int icRemapping;

    QVector<ColorCycling *> colorcyclings;
    int icColorCycling;


    char background;

    Data();
public:
    static Data * getInstance();

    void setGrp(Grp *);
    void appendWpe(Wpe *);
    void appendMapping (Mapping *);
    void appendRemapping (Remapping *);
    void appendColorCycling (ColorCycling *);

};

#endif // DATA_H
