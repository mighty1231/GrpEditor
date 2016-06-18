/* Class that manages wpe files */
#ifndef WPE_H
#define WPE_H

#include <QVector>
#include <QRgb>
#include <QFile>

class Wpe
{
private:
    QRgb data[256];
    QString name;

    Wpe(QByteArray ba);
public:
    static Wpe * load(QString name, QString fname);
    static Wpe * loadDefault();

    QString getName();
    QRgb getColor (int index) {return data[index];}
    QRgb const& operator[](int index) const;
};

#endif // WPE_H
