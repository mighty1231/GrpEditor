#include "dragframecommand.h"

#include <QListWidget>
#include <QDebug>
#include "component/grp.h"
#include "widget/mainwindow.h"

DragFrameCommand::DragFrameCommand(MainWindow *mw,
                                   int from,
                                   int to,
                                   QUndoCommand *parent)
    : QUndoCommand(parent), mw(mw), from(from), to(to)
{
#ifdef QT_DEBUG
    qDebug() << "DragFrameCommand#" << this
             << " [" << from << ", " << to << "]";
#endif

    this->listWidget = mw->getFrameListWidget();

    Q_ASSERT(listWidget->currentRow() == mw->getFrameIndex());
}

void DragFrameCommand::undo()
{
#ifdef QT_DEBUG
    qDebug() << "DragFrameCommand#" << this
             << " [" << from << ", " << to << "].undo()";
#endif

    drag(to, from);
}


void DragFrameCommand::redo()
{
#ifdef QT_DEBUG
    qDebug() << "DragFrameCommand#" << this
             << " [" << from << ", " << to << "].redo()";
#endif

    drag(from, to);
}

void DragFrameCommand::drag(int from, int to)
{
    int prevIndex = mw->getFrameIndex();
    int newIndex;

    if (from < to) {
        mw->getGrp()->dragFrame(from, to);
        listWidget->insertItem(to, listWidget->takeItem(from));
        if (from <= prevIndex && prevIndex <= to) {
            newIndex = (from != prevIndex) ? prevIndex-1 : to;

            listWidget->blockSignals(true);
            mw->setFrameIndex(newIndex);
            listWidget->setCurrentRow(newIndex);
            listWidget->blockSignals(false);
        }
    } else { // from > to
        mw->getGrp()->dragFrame(from, to);
        listWidget->insertItem(to, listWidget->takeItem(from));
        if (to <= prevIndex && prevIndex <= from) {
            newIndex = (from != prevIndex) ? prevIndex+1 : to;

            listWidget->blockSignals(true);
            mw->setFrameIndex(newIndex);
            listWidget->setCurrentRow(newIndex);
            listWidget->blockSignals(false);
        }
    }
}
