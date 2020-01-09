#ifndef HISTORY_H
#define HISTORY_H

#include "cell_manager.h"
const int UNDO_PTR_NOT_DEFINED = -10;
class PainterIntf
{
public:
    virtual void restruct(CellManager*) = 0;
};

class History
{
    vector<CellManager*> mMgrArr;
    CellManager mStartMgr;
    PainterIntf *mPainterIntf;
    int mUndoIndexPointer;

    void reduceToPointer();

public:
    History(PainterIntf* intf);
    ~History() { clean(); }
    void addNewMap(CellManager* mgr);

    void undo();
    void redo();
    void clean();
    bool canUndo();
    bool canRedo();
};

#endif // HISTORY_H
