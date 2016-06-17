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
