#ifndef SWAPFRAMECOMMAND_H
#define SWAPFRAMECOMMAND_H

#include <QUndoCommand>

class MainWindow;
class QListWidget;

class SwapFrameCommand : public QUndoCommand
{
public:
    SwapFrameCommand(MainWindow *, int, int,
                     QUndoCommand *parent = 0);

    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;

private:
    MainWindow *mw;
    int index1; // lower
    int index2; // higher
    QListWidget *listWidget;

    void swap();
};

#endif // SWAPFRAMECOMMAND_H
