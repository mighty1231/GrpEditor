#ifndef MAPPING_H
#define MAPPING_H

#include <QFile>

class Mapping
{
private:
    char table[256];
    QString name;

    Mapping(QByteArray ba);
public:
    static Mapping * load(QString name, QString fname);
    static Mapping * loadDefault();

    QString getName();
    quint8 getIndex(int index);
};

#endif // MAPPING_H
