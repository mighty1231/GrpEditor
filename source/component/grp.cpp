#include "grp.h"

Grp::Grp(int framecount, int width, int height)
{
    Q_ASSERT(0 < framecount && framecount < 65536);
    Q_ASSERT(0 < width && width < 65536);
    Q_ASSERT(0 < height && height < 65536);

    this->framecount = framecount;
    this->width = width;
    this->height = height;
    for (int i=0; i<framecount; i++) {
        this->frames.append(new QByteArray(width*height, 0));
    }
}
Grp::Grp(short framecount, short width, short height)
{
    Q_ASSERT(framecount != 0);
    Q_ASSERT(width != 0);
    Q_ASSERT(height != 0);

    this->framecount = framecount;
    this->width = width;
    this->height = height;
    for (int i=0; i<framecount; i++) {
        this->frames.append(new QByteArray(width*height, 0));
    }
}
Grp::~Grp()
{
    for (int i=0; i<framecount; i++) {
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

    if (header->count == 0 || header->width == 0 || header->height == 0)
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
                memcpy(grp->frames[i]->data(), grp->frames[j]->constData(),
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
            char *dest = grp->frames[i]->data()+(frameHeader->y+line)*header->width+frameHeader->x;
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

Grp *Grp::loadEmpty(int _width, int _height)
{
    if (_width < 0 || _width > 65536
            || _height < 0 || _height > 65536) {
        return NULL;
    }

    Grp *grp = new Grp(1, _width, _height);
    return grp;
}

void Grp::save(QString fname)
{
    QFile file(fname);

    if (!file.open(QIODevice::WriteOnly)) {
        return;
    }
    GrpHeader header = {framecount, width, height};
    QVector<GrpFrameHeader> frameHeaders(framecount);

    QByteArray data;
    int dataOffset = sizeof(header)+framecount*sizeof(GrpFrameHeader);

    QVector<QByteArray> frameHashes;

    for (int fnum=0; fnum<framecount; fnum++) {
        QByteArray *frame = frames[fnum];

        // get border
        int x1, x2, y1, y2;
        int w, h; // after calculate x1, x2, y1, y2, fill it.

        // top
        int __inc_i;
        for (__inc_i=0; __inc_i<width*height; __inc_i++) {
            if (frame->at(__inc_i) != 0)
                break;
        }
        // empty frame
        if (__inc_i == width*height) {
            frameHeaders[fnum].x = 0;
            frameHeaders[fnum].y = 0;
            frameHeaders[fnum].w = 0;
            frameHeaders[fnum].h = 0;
            frameHeaders[fnum].offset = 0;
            frameHashes.append(QByteArray());
            continue;
        }
        y1 = __inc_i / width;

        // bottom
        int __dec_i = width*height-1;
        while (frame->at(__dec_i) == 0)
            __dec_i--;
        y2 = __dec_i / width;
        h = y2-y1+1;

        // left
        x1 = 0;
        while(1) {
            int offset = y1*width+x1;
            int i = h;
            for (; i>0; i--, offset+=width) {
                if (frame->at(offset) != 0)
                    break;
            }
            if (i != 0)
                break;
            x1++;
        }

        // right
        x2 = width-1;
        while(1) {
            int offset = y1*width+x2;
            int i=h;
            for (; i>0; i--, offset+=width) {
                if (frame->at(offset) != 0)
                    break;
            }
            if (i != 0)
                break;
            x2--;
        }
        w = x2-x1+1;

        // fill border infos
        frameHeaders[fnum].x = x1;
        frameHeaders[fnum].y = y1;
        frameHeaders[fnum].w = w;
        frameHeaders[fnum].h = h;

        // hash
        QCryptographicHash hash(QCryptographicHash::Sha3_512);
        for (int y=y1; y<=y2; y++) {
            hash.addData(frame->data()+y*width+x1, w);
        }
        QByteArray hashResult = hash.result();
        int prevIndex = frameHashes.indexOf(hashResult);
        if (!frameHashes.empty() && prevIndex != -1) {
            frameHeaders[fnum].offset = frameHeaders[prevIndex].offset;
            frameHashes.append(hashResult);
            continue;
        }
        frameHeaders[fnum].offset = dataOffset;
        frameHashes.append(hashResult);


        // lines
        QVector<unsigned short> lineOffsets(h);
        QVector<QByteArray> lineHashes;
        unsigned short lineDataOffset = h*sizeof(unsigned short);
        QByteArray lineData;

        for (int y=y1; y<=y2; y++) {
            QCryptographicHash lineHash(QCryptographicHash::Sha3_512);
            lineHash.addData(frame->data()+y*width+x1, w);
            QByteArray lineHashResult = lineHash.result();
            int prevLineIndex = lineHashes.indexOf(lineHashResult);
            if (!lineHashes.empty() && prevLineIndex != -1) {
                lineOffsets[y-y1] = lineOffsets[prevLineIndex];
                lineHashes.append(lineHashResult);
                continue;
            }
            lineOffsets[y-y1] = lineDataOffset;
            lineHashes.append(lineHashResult);

            // encode line.
            // meet three consecutive character with nonzero, pack them.
            enum EncMode {EMPTY, REPEAT1, REPEAT2, REPEAT3};
            EncMode mode = EMPTY;
            char *cur = frame->data()+y*width+x1;

            int x = x1;
            char lastChar;
            unsigned char curBlockSize;
            while (x <= x2) {
                switch (mode) {
                case EMPTY:
                    curBlockSize = 1;
                    mode = REPEAT1;
                    break;
                case REPEAT1:
                    if (*cur == lastChar) {
                        if (++curBlockSize == 0x3F) {
                            // Preferred to repeat
                            lineData.append(0x3D);
                            lineData.append(cur-0x3E, 0x3D);
                            lineDataOffset += 0x3E;
                            curBlockSize = 2;
                        } else if (lastChar == 0 && curBlockSize == 2) {
                            /* special case, start with \0\0 -> REPEAT3 */
                            mode = REPEAT3;
                        } else {
                            mode = REPEAT2;
                        }
                    } else {
                        if (++curBlockSize == 0x3F) {
                            lineData.append(0x3F);
                            lineData.append(cur-0x3E, 0x3F);
                            lineDataOffset += 0x40;
                            mode = EMPTY;
                        }
                    }
                    break;
                case REPEAT2:
                    if (*cur == lastChar) {
                        mode = REPEAT3;
                        // pack
                        if (curBlockSize != 2) {
                            lineData.append(curBlockSize-2);
                            lineData.append(cur-curBlockSize, curBlockSize-2);
                            lineDataOffset += curBlockSize-1;
                        }
                        curBlockSize = 3;
                    } else {
                        if (++curBlockSize == 0x3F) {
                            lineData.append(0x3F);
                            lineData.append(cur-0x3E, 0x3F);
                            lineDataOffset += 0x40;
                            mode = EMPTY;
                        } else {
                            mode = REPEAT1;
                        }
                    }
                    break;
                case REPEAT3:
                    if (*cur == lastChar) {
                        curBlockSize++;
                        if (lastChar == 0 && curBlockSize == 0x7F) {
                            lineData.append(-1);
                            lineDataOffset += 1;
                            mode = EMPTY;
                        } else if (lastChar != 0 && curBlockSize == 0x3F) {
                            lineData.append(0x7F);
                            lineData.append(lastChar);
                            lineDataOffset += 2;
                            mode = EMPTY;
                        }
                    } else {
                        // pack
                        if (lastChar == 0) {
                            lineData.append(0x80 | curBlockSize);
                            lineDataOffset += 1;
                        } else {
                            lineData.append(0x40 | curBlockSize);
                            lineData.append(lastChar);
                            lineDataOffset += 2;
                        }
                        curBlockSize = 1;
                        mode = REPEAT1;
                    }
                }

                x++;
                lastChar = *(cur++);
            }

             Q_ASSERT(lineData.length() == lineDataOffset - 2*h);

            // last
            switch (mode) {
            case EMPTY:
                break;
            case REPEAT2:
                if (lastChar == 0) {
                    /* special case, end with \0\0 -> 0x82! */
                    if (curBlockSize > 2) {
                        lineData.append(curBlockSize-2);
                        lineData.append(cur-curBlockSize, curBlockSize-2);
                        lineDataOffset += curBlockSize-1;
                    }
                    lineData.append(-126);
                    lineDataOffset += 1;
                    break;
                }
            case REPEAT1:
                lineData.append(curBlockSize);
                lineData.append(cur-curBlockSize, curBlockSize);
                lineDataOffset += curBlockSize+1;
                break;
            case REPEAT3:
                if (lastChar == 0) {
                    lineData.append(0x80 | curBlockSize);
                    lineDataOffset += 1;
                } else {
                    lineData.append(0x40 | curBlockSize);
                    lineData.append(lastChar);
                    lineDataOffset += 2;
                }
            }
        }

        data.append((const char *)lineOffsets.data(), h*sizeof(unsigned short));
        data.append(lineData);
        dataOffset += h*sizeof(unsigned short);
        dataOffset += lineData.length();
    }

    QDataStream out(&file);
    out.writeRawData((const char *)&header, sizeof(header));
    out.writeRawData((const char *)frameHeaders.constData(), framecount*sizeof(GrpFrameHeader));
    out.writeRawData(data.constData(), data.length());
    file.close();
}

/* Make empty frame to i-th position. */
void Grp::insertFrame(int i)
{
    Q_ASSERT(0 <= i && i <= framecount);

    frames.insert(i, new QByteArray(width * height, 0));
    framecount++;
}

void Grp::insertFrame(int i, QByteArray qb)
{
    Q_ASSERT(0 <= i && i <= framecount);

    frames.insert(i, new QByteArray(qb));
    framecount++;
}

void Grp::copyFrame(int i)
{
    Q_ASSERT(0 <= i && i < framecount);

    frames.insert(i, new QByteArray(*frames[i]));
    framecount++;
}

void Grp::deleteFrame(int i)
{
    Q_ASSERT(0 <= i && i < framecount);

    QByteArray *t = frames[i];
    frames.erase(frames.begin()+i);
    delete t;
    framecount--;
}

void Grp::swapFrame(int i, int j)
{
    Q_ASSERT(0 <= i && i < framecount);
    Q_ASSERT(0 <= j && j < framecount);
    Q_ASSERT(i != j);

    QByteArray *tmp = frames[i];
    frames[i] = frames[j];
    frames[j] = tmp;
}

void Grp::upmostFrame(int i)
{
    Q_ASSERT(0 < i && i < framecount);

    QByteArray *tmp = frames[i];
    for (int c=i; c>0; c--) {
        frames[c] = frames[c-1];
    }
    frames[0] = tmp;
}

void Grp::downmostFrame(int i)
{
    Q_ASSERT(0 <= i && i < framecount-1);


    QByteArray *tmp = frames[i];
    for (int c=i; c<framecount-1; c++) {
        frames[c] = frames[c+1];
    }
    frames[framecount-1] = tmp;
}
