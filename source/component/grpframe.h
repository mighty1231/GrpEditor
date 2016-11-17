#ifndef GRPFRAME_H
#define GRPFRAME_H

#include <QtCore>

class GrpFrame
{
private:
    QString name;
    int width, height;
    QByteArray *content;

public:
    ~GrpFrame();

    GrpFrame(QString name, int width, int height);
    GrpFrame(QString name, int width, int height, QByteArray *ba);
    GrpFrame(const GrpFrame &);

    char at(int t);
    char at(int i, int j);
    char * scanLine(int j);

    char * data();
    const char * constData();

    QString getName();
    QByteArray * getContent();

    GrpFrame & operator=(const GrpFrame &frame);
};

#endif // GRPFRAME_H
