#include "remapping.h"
#include "globals.h"

Remapping::Remapping(QByteArray ba)
{
    int length = ba.length();
    Q_ASSERT ((length & 0xFF) == 0);
    Q_ASSERT (0 < length && length <= 256*256);

    size = length / 256;
    data = ba;
}

Remapping * Remapping::load(QString name, QString fname)
{
    QFile file(fname);

    if (!file.open(QIODevice::ReadOnly)) {
        return NULL;
    }

    QByteArray ba = file.readAll();
    file.close();

    int length = ba.length();
    if ((length & 0xFF) != 0 || length == 0 || length > 256*256) {
        return NULL;
    }

    Remapping *remapping = new Remapping(ba);
    remapping->name = name;
    return remapping;
}

Remapping * Remapping::loadDefault()
{
    QByteArray ba(256*256, 0);
    for (int i=0; i<256; i++) {
        for (int j=0; j<256; j++) {
            ba[(i<<8)+j] = i;
        }
    }
    Remapping *remapping = new Remapping(ba);
    remapping->name = DEFAULT_OBJNAME;
    return remapping;
}

QString Remapping::getName()
{
    return name;
}
int Remapping::getSize()
{
    return size;
}

QByteArray * Remapping::allocateTableWithBackground(int index)
{
    QByteArray * ba = new QByteArray(size, 0);
    for (int i=0; i<size; i++) {
        ba->data()[i] = data[index];
        index += 256;
    }
    return ba;
}
