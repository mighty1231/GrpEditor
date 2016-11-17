#include "createframecommand.h"

#include <QListWidget>
#include <QDebug>
#include "component/grp.h"
#include "widget/mainwindow.h"

CreateFrameCommand::CreateFrameCommand(MainWindow *mw, int index,
                                       const GrpFrame& frame,
                                       QUndoCommand *parent)
    : QUndoCommand(parent), mw(mw), index(index), frame(frame)
{
#ifdef QT_DEBUG
    qDebug() << "CreateFrameCommand#" << this << " [" << index << "]";
#endif

    this->listWidget = mw->getFrameListWidget();

    Q_ASSERT(listWidget->currentRow() == mw->getFrameIndex());
}

void CreateFrameCommand::undo()
{
#ifdef QT_DEBUG
    qDebug() << "CreateFrameCommand#" << this << " [" << index << "].undo()";
#endif

    Q_ASSERT(listWidget->currentRow() == mw->getFrameIndex());
    mw->getGrp()->deleteFrame(index);
    int prevIndex = mw->getFrameIndex();

    listWidget->blockSignals(true);
    delete listWidget->takeItem(index);
    if (prevIndex >= index) {
        listWidget->setCurrentRow(prevIndex - 1);
        mw->setFrameIndex(prevIndex - 1);
    }
    listWidget->blockSignals(false);

    mw->updatePixel();
}

void CreateFrameCommand::redo()
{
#ifdef QT_DEBUG
    qDebug() << "CreateFrameCommand#" << this << " [" << index << "].redo()";
#endif

    Q_ASSERT(listWidget->currentRow() == mw->getFrameIndex());
    mw->getGrp()->insertFrame(index, new GrpFrame(frame));
    mw->setFrameIndex(index);

    listWidget->insertItem(index, frame.getName());
    listWidget->blockSignals(true);
    listWidget->setCurrentRow(index);
    listWidget->blockSignals(false);

    mw->updatePixel();
}
