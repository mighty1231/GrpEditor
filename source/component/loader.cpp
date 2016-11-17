#include "loader.h"
#include "widget/mainwindow.h"
#include "component/wpe.h"
#include "component/mapping.h"
#include "component/remapping.h"
#include "component/colorcycling.h"

ComponentLoader::ComponentLoader(MainWindow *mw) : colorTable(256)
{
    icWpe = 0;
    icMapping = 0;
    icRemapping = 0;
    icColorCycling = 0;

    overflowedColor = mw->getOverflowedColor();

    loadWpe();
    loadMapping();
    loadRemapping();
    loadColorCycling();

    QTimer::singleShot(CYCLING_PERIOD, this, SLOT(cyclingTick()));

    updateColorTable();
}

void ComponentLoader::loadWpe()
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

void ComponentLoader::loadMapping()
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

void ComponentLoader::loadRemapping()
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

void ComponentLoader::loadColorCycling()
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

QVector<QRgb> const &ComponentLoader::getColorTable() const
{
    return colorTable;
}

// @TODO: background index
// @TODO: mapping would be called to load entire table to do more perf.
void ComponentLoader::updateColorTable()
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
            colorTable[i] = mw->getOverflowedColor();
    }
    delete remapTable;
    emit colorTableChanged();
}

void ComponentLoader::cyclingTick()
{
    if (colorCyclings[icColorCycling]->processTick()){
        updateColorTable();
    }

    QTimer::singleShot(CYCLING_PERIOD, this, SLOT(cyclingTick()));
}
