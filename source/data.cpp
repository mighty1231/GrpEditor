#include "data.h"

Data * Data::instance = NULL;
Data::Data() : colorTable(256)
{
    grp = NULL;

    icWpe = 0;
    icMapping = 0;
    icRemapping = 0;
    icColorCycling = 0;

    drawingIndex = 0;
    overflowedColor = qRgb(255, 0, 255);

    QTimer::singleShot(CYCLING_PERIOD, this, SLOT(cyclingTick()));
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

// @TODO: background index
// @TODO: mapping would be called to load entire table to do more perf.
void Data::updateColorTable()
{
    Remapping *remapping = remappings[icRemapping];
    int _rem_size = remapping->getSize();
    QByteArray *remapTable = remapping->allocateTableWithBackground(0);

    int _mapped;
    for (int i=0; i<256; i++){
        _mapped = mappings[icMapping]->getIndex(i);
        if (_mapped < _rem_size)
            colorTable[i] = wpes[icWpe]->getColor(
                        colorCyclings[icColorCycling]->getIndex(
                            (unsigned char)(remapTable->at(_mapped))));
        else
            colorTable[i] = overflowedColor;
    }
    delete remapTable;
    emit colorTableChanged();
}

void Data::cyclingTick()
{
#ifdef QT_DEBUG
//        qDebug() << "SLOT Data::cyclingTick";
#endif
    if (colorCyclings[icColorCycling]->processTick()){
        updateColorTable();
    }


    QTimer::singleShot(CYCLING_PERIOD, this, SLOT(cyclingTick()));
}
