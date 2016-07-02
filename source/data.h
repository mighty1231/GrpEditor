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
    /* Singleton Type */
    static Data * instance;

    /* objects
     * and index of current selected one of them */
    Grp *grp;
    QString grpPath;

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
    QRgb overflowedColor;

    Data();
public:
    static Data * getInstance();

    Grp * getGrp() {return grp;}
    void setGrp(Grp *grp) {this->grp=grp;}
    QString getGrpPath() {return grpPath;}
    void setGrpPath(QString qs) {grpPath = qs;}

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
    QRgb getOverflowedColor() {return overflowedColor;}

signals:
    void colorTableChanged();

public slots:
    void setWpeIndex (int i) {
#ifdef QT_DEBUG
        qDebug() << "SLOT Data::setWpeIndex";
#endif
        if (icWpe == i)
            return;
        icWpe = i;

        updateColorTable();
    }
    void setMappingIndex (int i) {
#ifdef QT_DEBUG
        qDebug() << "SLOT Data::setMappingIndex";
#endif
        if (icMapping == i)
            return;
        icMapping = i;

        updateColorTable();
    }
    void setRemappingIndex (int i) {
#ifdef QT_DEBUG
        qDebug() << "SLOT Data::setRemappingIndex";
#endif
        if (icRemapping == i)
            return;
        icRemapping = i;

        updateColorTable();
    }
    void setColorCyclingIndex (int i) {
#ifdef QT_DEBUG
        qDebug() << "SLOT Data::setColorCyclingIndex";
#endif
        if (icColorCycling == i)
            return;
        icColorCycling = i;

        updateColorTable();
    }
    void setDrawingIndex (int i) {
#ifdef QT_DEBUG
        qDebug() << "SLOT Data::setDrawingIndex";
#endif
        drawingIndex = i;

        updateColorTable();
    }
    void setOverflowR (int i) {
#ifdef QT_DEBUG
        qDebug() << "SLOT Data::setOverflowR";
#endif
        int r, g, b;
        r = i;
        g = qGreen(overflowedColor);
        b = qBlue(overflowedColor);
        overflowedColor = qRgb(r, g, b);

        if (remappings[icRemapping]->getSize() != 256)
            updateColorTable();
    }
    void setOverflowG (int i) {
#ifdef QT_DEBUG
        qDebug() << "SLOT Data::setOverflowG";
#endif
        int r, g, b;
        r = qRed(overflowedColor);
        g = i;
        b = qBlue(overflowedColor);
        overflowedColor = qRgb(r, g, b);

        if (remappings[icRemapping]->getSize() != 256)
            updateColorTable();
    }
    void setOverflowB (int i) {
#ifdef QT_DEBUG
        qDebug() << "SLOT Data::setOverflowB";
#endif
        int r, g, b;
        r = qRed(overflowedColor);
        g = qGreen(overflowedColor);
        b = i;
        overflowedColor = qRgb(r, g, b);

        if (remappings[icRemapping]->getSize() != 256)
            updateColorTable();
    }
};

#endif // DATA_H
