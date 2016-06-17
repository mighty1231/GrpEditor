#ifndef REMAPPING_H
#define REMAPPING_H

#include <QtCore>

class Remapping
{
private:
    int size; // size of remapping. it could be 1 through 256.
    QByteArray data;

    QString name;

    Remapping(QByteArray ba);
public:
    static Remapping * load(QString name, QString fname);
    static Remapping * loadDefault();

    QString getName();
    int getSize();
    QByteArray * getTableOnBackground(int index);
};

#endif // REMAPPING_H
