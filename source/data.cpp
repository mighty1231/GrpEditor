#include "data.h"

Data * Data::instance = NULL;
Data::Data() : colorTable(256)
{
    grp = NULL;
    icGrp = 0;


    icWpe = 0;
    icMapping = 0;
    icRemapping = 0;
    icColorCycling = 0;

    drawingIndex = 0;
}
Data * Data::getInstance()
{
    if (instance == NULL) {
        instance = new Data();
    }
    return instance;
}

QVector<QRgb> const &Data::getColorTable() const
{
    return colorTable;
}

void Data::updateColorTable()
{
    for (int i=0; i<256; i++){
        colorTable[i] = wpes[icWpe]->getColor(
                    mappings[icMapping]->getIndex(i)
                    );
    }
    emit colorTableChanged(colorTable);
}
