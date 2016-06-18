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

    QVector<QRgb> colorTable;

    // brush things
    int drawingIndex;

    Data();
public:
    static Data * getInstance();

    Grp * getGrp() {return grp;}
    void setGrp(Grp *grp) {
        this->grp=grp;
        this->icGrp=0;
        emit grpChanged(grp->getWidth(), grp->getHeight(), grp->getFrame(icGrp)->data());
    }
    int getGrpIndex() {return icGrp;}

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

    QVector<QRgb> const &getColorTable() const;
    void updateColorTable ();

    int getDrawingIndex() {return drawingIndex;}

signals:
    void grpChanged(int w, int h, char *data);
    void colorTableChanged(QVector<QRgb> colorTable);

public slots:
    void setGrpIndex(int i) {
        this->icGrp = i;
        emit grpChanged(grp->getWidth(), grp->getHeight(), grp->getFrame(icGrp)->data());
    }
    void setWpeIndex (int i) {
        if (icWpe == i)
            return;
        icWpe = i;
        updateColorTable();
    }
    void setMappingIndex (int i) {
        if (icMapping == i)
            return;
        icMapping = i;
        updateColorTable();
    }
    void setRemappingIndex (int i) {
        if (icRemapping == i)
            return;
        icRemapping = i;
        updateColorTable();
    }
    void setColorCyclingIndex (int i) {
        if (icColorCycling == i)
            return;
        icColorCycling = i;
        updateColorTable();
    }
    void setDrawingIndex (int i) {drawingIndex = i;}
};

#endif // DATA_H
