#ifndef DATA_H
#define DATA_H

#include "wpe.h"
#include "mapping.h"
#include "remapping.h"
#include "colorcycling.h"
#include "grp.h"


class Data : public QObject
{
    Q_OBJECT

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

    QVector<ColorCycling *> colorCyclings;
    int icColorCycling;


    char background;

    Data();
public:
    static Data * getInstance();

    void setGrp(Grp *grp) {this->grp = grp;}
    void appendWpe(Wpe *wpe) {wpes.append(wpe);}
    void appendMapping (Mapping *map) {mappings.append(map);}
    void appendRemapping (Remapping *rem) {remappings.append(rem);}
    void appendColorCycling (ColorCycling *cc) {colorCyclings.append(cc);}

    QVector<Wpe *> const &getWpes() const {return wpes;}
    QVector<Mapping *> const &getMappings() const {return mappings;}
    QVector<Remapping *> const &getRemappings() const {return remappings;}
    QVector<ColorCycling *> const &getColorCyclings() const {return colorCyclings;}

    int getWpeIndex () {return icWpe;}
    int getMappingIndex () {return icMapping;}
    int getRemappingIndex () {return icRemapping;}
    int getColorCyclingIndex () {return icColorCycling;}

public slots:
    void setWpeIndex (int i) {icWpe = i;}
    void setMappingIndex (int i) {icMapping = i;}
    void setRemappingIndex (int i) {icRemapping = i;}
    void setColorCyclingIndex (int i) {icColorCycling = i;}
};

#endif // DATA_H
