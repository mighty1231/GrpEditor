#ifndef DRAGFRAMECOMMAND_H
#define DRAGFRAMECOMMAND_H

/* DragFrameCommand(mw, from, to)
 *
 * case 1. from < to
 *   before : from   from+1 from+2 ... to-2 to-1 to
 *             ^-------------------...----------->
 *   after  : from+1 from+2 from+3 ... to-1 to   from
 *
 * case 2. from > to
 *   before : to   to+1 to+2 ... from-2 from-1 from
 *              <------------...----------------^
 *   after  : from to   to+1 ... from-3 from-2 from-1
 */

#include <QUndoCommand>

class MainWindow;
class QListWidget;

class DragFrameCommand : public QUndoCommand
{
public:
    DragFrameCommand(MainWindow *, int, int,
                     QUndoCommand *parent = 0);

    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;

private:
    MainWindow *mw;
    int from;
    int to;
    QListWidget *listWidget;

    void drag(int, int);
};

#endif // DRAGFRAMECOMMAND_H
