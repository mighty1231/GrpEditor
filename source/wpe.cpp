#include "wpe.h"
#include "globals.h"
#include <QtCore>

Wpe::Wpe(QByteArray ba)
{
    Q_ASSERT (ba.length() == 256 * 4);
    for (int i=0; i<256; i++) {
        data[i] = qRgb(ba[4*i], ba[4*i+1], ba[4*i+2]);
    }
}
Wpe * Wpe::load(QString name, QString fname)
{
    QFile file(fname);

    if (!file.open(QIODevice::ReadOnly)) {
        return NULL;
    }

    QByteArray ba = file.readAll();
    file.close();

    if (ba.length() != 256 * 4) {
        return NULL;
    }

    Wpe *wpe = new Wpe(ba);
    wpe->name = name;
    return wpe;
}

Wpe * Wpe::loadDefault()
{
    QByteArray ba(256 * 4, 0);
    for (int i = 0; i < 256; i++) {
        ba[4*i+1] = ba[4*i+1] = ba[4*i+2] = i;
    }
    Wpe *wpe = new Wpe(ba);
    wpe->name = DEFAULT_OBJNAME;
    return wpe;
}

QString Wpe::getName()
{
    return name;
}

QRgb const& Wpe::operator[](int index) const
{
    Q_ASSERT(index >= 0 && index < 256);
    return data[index];
}
