#include "mapping.h"
#include "globals.h"

Mapping::Mapping(QByteArray ba)
{
    Q_ASSERT (ba.length() == 256);
    memcpy(table, ba.data(), 256);
}
Mapping * Mapping::load(QString name, QString fname)
{
    QFile file(fname);

    if (!file.open(QIODevice::ReadOnly)) {
        return NULL;
    }

    QByteArray ba = file.readAll();
    file.close();

    if (ba.length() != 256) {
        return NULL;
    }

    Mapping *map = new Mapping(ba);
    map->name = name;
    return map;
}

Mapping * Mapping::loadDefault()
{
    QByteArray ba(256, 0);
    for (int i = 0; i < 256; i++) {
        ba[i] = i;
    }
    Mapping *map = new Mapping(ba);
    map->name = DEFAULT_OBJNAME;
    return map;
}

QString Mapping::getName()
{
    return name;
}

quint8 Mapping::getIndex(int index)
{
    Q_ASSERT(index >= 0 && index < 256);
    return table[index];
}

