#include "data.h"

Data * Data::instance = NULL;
Data::Data()
{
    grp = NULL;
    icGrp = 0;


    icWpe = 0;
    icMapping = 0;
    icColorCycling = 0;

    background = 0;
}
Data * Data::getInstance()
{
    if (instance == NULL) {
        instance = new Data();
    }
    return instance;
}

void Data::setGrp(Grp *grp)
{
    this->grp = grp;
}

void Data::appendWpe(Wpe *wpe)
{
    wpes.append(wpe);
}

void Data::appendMapping (Mapping *mapping)
{
    mappings.append(mapping);
}

void Data::appendRemapping (Remapping *remapping)
{
    remappings.append(remapping);
}

void Data::appendColorCycling (ColorCycling *colorcycling)
{
    colorcyclings.append(colorcycling);
}
