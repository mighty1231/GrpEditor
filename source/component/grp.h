#ifndef GRP_H
#define GRP_H

#include <QtCore>

class GrpFrame;

class Grp
{
private:
    int framecount;
    int width;
    int height;

    QVector<GrpFrame *> frames;

    Grp(int framecount, int width, int height);
    Grp(short framecount, short width, short height);

    struct GrpHeader {
        unsigned short count;
        unsigned short width;
        unsigned short height;
    };
    struct GrpFrameHeader {
        unsigned char x, y, w, h;
        unsigned int offset;
    };

public:
    ~Grp();
    static Grp * load(QString fname);
    static Grp * loadEmpty(int width, int height);
    void save(QString fname);

    int getWidth() {return width;}
    int getHeight() {return height;}
    int getFrameCount() {return framecount;}
    GrpFrame * getFrame(int i) {return frames[i];}

    void insertFrame(int i);
    void insertFrame(int i, GrpFrame *frame);
    void insertFrame(int i, const GrpFrame &frame);
    void copyFrame(int i);
    void deleteFrame(int i);
    void swapFrame(int i, int j);
    void upmostFrame(int i);
    void downmostFrame(int i);
};

#endif // GRP_H
