#ifndef CREATEFRAMECOMMAND_H
#define CREATEFRAMECOMMAND_H

#include <QUndoCommand>
#include "data.h"

class CreateFrameCommand : public QUndoCommand
{
public:
    CreateFrameCommand(int index, QByteArray *bdata, int, int,
                       QUndoCommand *parent = 0);
    ~CreateFrameCommand();

    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;

private:
    Data *data;

    int index;

    QByteArray *bdata;
    int width;
    int height;
};

#endif // CREATEFRAMECOMMAND_H
