#include "grpframe.h"

GrpFrame::~GrpFrame()
{
    delete content;
}

GrpFrame::GrpFrame(QString name, int width, int height)
{
    this->name = name;
    this->width = width;
    this->height = height;
    this->content = new QByteArray(width * height, 0);
}

GrpFrame::GrpFrame(QString name, int width, int height, QByteArray *ba)
{
    Q_ASSERT(ba->length() == width * height);

    this->name = name;
    this->width = width;
    this->height = height;
    this->content = ba;
}

GrpFrame::GrpFrame(const GrpFrame &frame)
{
    this->name = frame.name;
    this->width = frame.width;
    this->height = frame.height;
    this->content = new QByteArray(*frame.content);
}

char GrpFrame::at(int t)
{
    Q_ASSERT(0 <= t && t < width*height);

    return content->at(t);
}

char GrpFrame::at(int i, int j)
{
    Q_ASSERT(0 <= i && i < width);
    Q_ASSERT(0 <= j && j < height);

    return content->at(j * width + i);
}

char * GrpFrame::scanLine(int j)
{
    Q_ASSERT(0 <= j && j < height);

    return content->data()+j*width;
}

char * GrpFrame::data()
{
    return content->data();
}

const char *GrpFrame::constData()
{
    return content->constData();
}

QString GrpFrame::getName()
{
    return name;
}

QByteArray * GrpFrame::getContent()
{
    return new QByteArray(*content);
}

GrpFrame & GrpFrame::operator=(const GrpFrame & other)
{
    if (this != &other) {
        name = other.name;
        width = other.width;
        height = other.height;
        content = new QByteArray(*other.content);
    }
    return *this;
}
