#include "grp.h"

Grp::Grp(unsigned int framecount, unsigned int width, unsigned int height)
{
    this->framecount = framecount;
    this->width = width;
    this->height = height;
    for (unsigned int i=0; i<framecount; i++) {
        this->frames.append(new QByteArray(width*height, 0));
    }
}
Grp::Grp(unsigned short framecount, unsigned short width, unsigned short height)
{
    this->framecount = framecount;
    this->width = width;
    this->height = height;
    for (unsigned int i=0; i<framecount; i++) {
        this->frames.append(new QByteArray(width*height, 0));
    }
}
Grp::~Grp()
{
    for (unsigned int i=0; i<framecount; i++) {
        delete this->frames[i];
    }
}

Grp * Grp::load(QString fname)
{
    QFile file(fname);

    if (!file.open(QIODevice::ReadOnly)) {
        return NULL;
    }

    QByteArray ba = file.readAll();
    file.close();

    unsigned int fileLength = (unsigned int) ba.length();
    if (fileLength < 6)
        return NULL;
    GrpHeader *header = reinterpret_cast<GrpHeader *> (ba.data());
    qDebug() << "frame count" << header->count;
    qDebug() << "width" << header->width;
    qDebug() << "height" << header->height;

    if (fileLength < 6+sizeof(GrpFrameHeader)*header->count)
        return NULL;
    Grp *grp = new Grp(header->count, header->width, header->height);
    GrpFrameHeader *frameHeader = reinterpret_cast<GrpFrameHeader *>(ba.data()+6);
    QVector<GrpFrameHeader> history(header->count);
    for (unsigned int i=0; i<header->count; i++, frameHeader++) {
        /* read each frame. */

        /* Be more fast,
         * find in history */
        unsigned int j;
        for (j=0; j<i; j++) {
            if (memcmp(&history[j], frameHeader, sizeof(GrpFrameHeader)) == 0) {
                /* history hit! */
                memcpy(grp->frames[i]->data(), grp->frames[j]->data(),
                       header->width*header->height);
                break;
            }
        }
        if (j != i)
            continue;
        /* history miss */
        memcpy(&history[i], frameHeader, sizeof(GrpFrameHeader));

        /* decode grp to pixel data */
        if (fileLength < frameHeader->offset
            || fileLength < frameHeader->offset + 2*frameHeader->h) {
            delete grp;
            return NULL;
        }
        unsigned short *lineOffsets = (unsigned short *)(ba.data() + frameHeader->offset);
        for (unsigned int line=0; line<frameHeader->h; line++) {
            /* read each line */
            unsigned char *dest = (unsigned char *)(grp->frames[i]->data()+frameHeader->y*header->height+frameHeader->x);
            unsigned int srcIndex = frameHeader->offset + lineOffsets[line];
            int written = 0;

            unsigned char ch;
            enum DecMode {DEFAULT, REPEAT, STREAM};
            DecMode mode = DEFAULT;
            unsigned char left = 0; // count of left bytes at REPEAT and STREAM mode.
            while (written < frameHeader->w && srcIndex < fileLength) {
                ch = (unsigned char)(ba.data()[srcIndex]);
                switch (mode) {
                case DEFAULT:
                    if (ch & 0x80) {
                        written += ch & 0x7F;
                        srcIndex++;
                    } else if (ch & 0x40) {
                        left = ch & 0x3F;
                        srcIndex++;
                        mode = REPEAT;
                    } else {
                        left = ch;
                        srcIndex++;
                        mode = STREAM;
                    }
                    break;
                case REPEAT:
                    dest[written++] = ch;
                    if (--left == 0){
                        mode = DEFAULT;
                        srcIndex++;
                    }
                    break;
                case STREAM:
                    dest[written++] = ch;
                    srcIndex++;
                    if (--left == 0)
                        mode = DEFAULT;
                }
            }
        }
    }


    return grp;
}
