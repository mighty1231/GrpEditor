/* Class that manages wpe files */
#ifndef WPE_H
#define WPE_H

#include <QVector>
#include <QColor>
#include <QFile>

class Wpe
{
private:
    QColor data[256];
    QString name;

    Wpe(QByteArray ba);
public:
    static Wpe * load(QString name, QString fname);
    static Wpe * loadDefault();

    QString getName();
    QColor const& operator[](int index) const;
};

#endif // WPE_H
