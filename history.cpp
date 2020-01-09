#include "history.h"

History::History(PainterIntf* intf)
: mPainterIntf(intf), mUndoIndexPointer(UNDO_PTR_NOT_DEFINED)
{
    addNewMap(&mStartMgr); // add start position
}

void History::addNewMap(CellManager* mgr)
{
    // if was done undo/redo, reset the position and reset the array
    if (mUndoIndexPointer != UNDO_PTR_NOT_DEFINED)
    {
        reduceToPointer();
        mUndoIndexPointer = UNDO_PTR_NOT_DEFINED;
    }

     mMgrArr.push_back(mgr->clone());
}

void History::reduceToPointer()
{
    while (int(mMgrArr.size()) - 1 > mUndoIndexPointer)
    {
        delete mMgrArr[mMgrArr.size() - 1];
        mMgrArr.pop_back();
    }
}

void History::undo() // must check for canUndo() before calling the func
{
    if (mUndoIndexPointer == UNDO_PTR_NOT_DEFINED)
    {
        unsigned int arrSize = mMgrArr.size();
        mUndoIndexPointer = arrSize - 2;
    }
    else mUndoIndexPointer--;

    //CellManager *mgr = (mUndoIndexPointer  == -1) ? (&mStartMgr) : mMgrArr[mUndoIndexPointer];
    mPainterIntf->restruct(mMgrArr[mUndoIndexPointer]);
}

void History::redo() // must check for canRedo() before calling the func
{
    mUndoIndexPointer++;
    mPainterIntf->restruct(mMgrArr[mUndoIndexPointer]);
}

void History::clean()
{
    for (CellManager* mgr : mMgrArr) delete mgr;
    mMgrArr.clear();
    addNewMap(&mStartMgr);
}

bool History::canUndo()
{
    if (mUndoIndexPointer == UNDO_PTR_NOT_DEFINED && mMgrArr.size() > 1) return true;
    if (mUndoIndexPointer > 0) return true;
    return false;
}

bool History::canRedo()
{
    if (mUndoIndexPointer != UNDO_PTR_NOT_DEFINED && mUndoIndexPointer < int(mMgrArr.size()) - 1) return true;
    return false;
}
