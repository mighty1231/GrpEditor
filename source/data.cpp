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

    loadWpe();
    loadMapping();
    loadRemapping();
    loadColorCycling();

    QTimer::singleShot(CYCLING_PERIOD, this, SLOT(cyclingTick()));
}

void Data::loadWpe()
{
    /* wpe */
    wpes.append(Wpe::loadDefault());
    QDir dir("data/wpe/");
    dir.setFilter(QDir::Files | QDir::NoDotAndDotDot);
    QStringList filters;
    filters << "*.wpe";
    dir.setNameFilters(filters);
    QDirIterator iter(dir);
    while (iter.hasNext()) {
        iter.next();
        Wpe *wpe = Wpe::load(iter.fileInfo().completeBaseName(),
                             iter.filePath());
        if (wpe == NULL) {
            /* fail */
        } else {
            wpes.append(wpe);
        }
    }
}

void Data::loadMapping()
{
    mappings.append(Mapping::loadDefault());
    QDir dir("data/mapping/");
    dir.setFilter(QDir::Files | QDir::NoDotAndDotDot);
    QStringList filters;
    filters << "*.mapping";
    dir.setNameFilters(filters);
    QDirIterator iter(dir);
    while (iter.hasNext()) {
        iter.next();
        Mapping *mapping = Mapping::load(iter.fileInfo().completeBaseName(),
                             iter.filePath());
        if (mapping == NULL) {
            /* fail */
        } else {
            mappings.append(mapping);
        }
    }
}

void Data::loadRemapping()
{
    remappings.append(Remapping::loadDefault());
    QDir dir("data/remapping/");
    dir.setFilter(QDir::Files | QDir::NoDotAndDotDot);
    QStringList filters;
    filters << "*.remapping";
    dir.setNameFilters(filters);
    QDirIterator iter(dir);
    while (iter.hasNext()) {
        iter.next();
        Remapping *remapping = Remapping::load(iter.fileInfo().completeBaseName(),
                             iter.filePath());
        if (remapping == NULL) {
            /* fail */
        } else {
            remappings.append(remapping);
        }
    }
}

void Data::loadColorCycling()
{
    colorCyclings.append(ColorCycling::loadDefault());
    QDir dir("data/colorcycling/");
    dir.setFilter(QDir::Files | QDir::NoDotAndDotDot);
    QStringList filters;
    filters << "*.colorcycling";
    dir.setNameFilters(filters);
    QDirIterator iter(dir);
    while (iter.hasNext()) {
        iter.next();
        ColorCycling *colorCycling = ColorCycling::load(iter.fileInfo().completeBaseName(),
                             iter.filePath());
        if (colorCycling == NULL) {
            /* fail */
        } else {
            colorCyclings.append(colorCycling);
        }
    }
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
