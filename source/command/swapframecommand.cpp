#include "swapframecommand.h"

#include <QListWidget>
#include <QDebug>
#include "component/grp.h"
#include "widget/mainwindow.h"

SwapFrameCommand::SwapFrameCommand(MainWindow *mw,
                                   int index1,
                                   int index2,
                                   QUndoCommand *parent)
    : QUndoCommand(parent), mw(mw)
{
#ifdef QT_DEBUG
    qDebug() << "SwapFrameCommand#" << this
             << " [" << index1 << ", " << index2 << "]";
#endif

    if (index1 < index2) {
        this->index1 = index1;
        this->index2 = index2;
    } else {
        this->index1 = index2;
        this->index2 = index1;
    }
    this->listWidget = mw->getFrameListWidget();

    Q_ASSERT(listWidget->currentRow() == mw->getFrameIndex());
}

void SwapFrameCommand::undo()
{
#ifdef QT_DEBUG
    qDebug() << "SwapFrameCommand#" << this
             << " [" << index1 << ", " << index2 << "].undo()";
#endif
    swap();
}

void SwapFrameCommand::redo()
{
#ifdef QT_DEBUG
    qDebug() << "SwapFrameCommand#" << this
             << " [" << index1 << ", " << index2 << "].redo()";
#endif
    swap();
}

void SwapFrameCommand::swap()
{
    mw->getGrp()->swapFrame(index1, index2);
    int prevIndex = mw->getFrameIndex();

    /* Ordering is important for below lines */
    listWidget->blockSignals(true);
    QListWidgetItem *higher = listWidget->takeItem(index2);
    QListWidgetItem *lower = listWidget->takeItem(index1);
    listWidget->insertItem(index1, higher);
    listWidget->insertItem(index2, lower);

    if (prevIndex == index1 || prevIndex == index2) {
        int newIndex = prevIndex ^ index1 ^ index2;

        mw->setFrameIndex(newIndex);
        listWidget->setCurrentRow(newIndex);
    }
    listWidget->blockSignals(false);

    // display doesn't change
    // mw->updatePixel();
}
