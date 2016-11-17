#include "deleteframecommand.h"

#include <QListWidget>
#include <QDebug>
#include "component/grp.h"
#include "component/grpframe.h"
#include "widget/mainwindow.h"

DeleteFrameCommand::DeleteFrameCommand(MainWindow *mw, int index,
                                       QUndoCommand *parent)
    : QUndoCommand(parent), mw(mw), index(index)
{
#ifdef QT_DEBUG
    qDebug() << "DeleteFrameCommand#" << this << " [" << index << "]";
#endif

    this->listWidget = mw->getFrameListWidget();
    this->frame = new GrpFrame(*mw->getGrp()->getFrame(index));

    Q_ASSERT(listWidget->currentRow() == mw->getFrameIndex());
}

DeleteFrameCommand::~DeleteFrameCommand()
{
    delete frame;
}

void DeleteFrameCommand::undo()
{
#ifdef QT_DEBUG
    qDebug() << "DeleteFrameCommand#" << this << " [" << index << "].undo()";
#endif

    Q_ASSERT(listWidget->currentRow() == mw->getFrameIndex());
    mw->getGrp()->insertFrame(index, new GrpFrame(*frame));
    mw->setFrameIndex(index);

    listWidget->insertItem(index, frame->getName());
    listWidget->blockSignals(true);
    listWidget->setCurrentRow(index);
    listWidget->blockSignals(false);

    mw->updatePixel();
}

void DeleteFrameCommand::redo()
{
#ifdef QT_DEBUG
    qDebug() << "DeleteFrameCommand#" << this << " [" << index << "].redo()";
#endif

    Q_ASSERT(listWidget->currentRow() == mw->getFrameIndex());
    mw->getGrp()->deleteFrame(index);
    int prevIndex = mw->getFrameIndex();

    listWidget->blockSignals(true);
    delete listWidget->takeItem(index);
    if (prevIndex > index) {
        listWidget->setCurrentRow(prevIndex - 1);
        mw->setFrameIndex(prevIndex - 1);
    }
    listWidget->blockSignals(false);

    mw->updatePixel();
}
