#include "createframecommand.h"

CreateFrameCommand::CreateFrameCommand(int index, QByteArray *bdata,
                                       QUndoCommand *parent)
    : QUndoCommand(parent), index(index), bdata(bdata)
{
    data = Data::getInstance();

}

CreateFrameCommand::~CreateFrameCommand()
{
    delete bdata;
}

void CreateFrameCommand::undo()
{

}

void CreateFrameCommand::redo()
{

}
