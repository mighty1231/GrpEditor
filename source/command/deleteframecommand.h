#ifndef DELETEFRAMECOMMAND_H
#define DELETEFRAMECOMMAND_H

#include <QUndoCommand>

class MainWindow;
class GrpFrame;
class QListWidget;

class DeleteFrameCommand : public QUndoCommand
{
public:
    DeleteFrameCommand(MainWindow *mw, int index,
                       QUndoCommand *parent = 0);
    ~DeleteFrameCommand();

    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;

private:
    MainWindow *mw;
    GrpFrame *frame;
    int index;
    QListWidget *listWidget;
};

#endif // DELETEFRAMECOMMAND_H
