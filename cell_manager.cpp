#include "cell_manager.h"
#include <iostream>

CellManager::CellManager() : mRegularStepCount(0)
{
    initGame(false);
    emptyCellSelection();
}

void CellManager::set(CellManager* mgr)
{
    CellVal cell_val = EMPTY_CELL_VAL;
    for (int rowN = 0; rowN < BOARD_LEN; rowN++)
    for (int clmnN = 0; clmnN < BOARD_LEN; clmnN++)
    {
        cell_val = mgr->mCellMatrix[rowN][clmnN].getVal();
        mCellMatrix[rowN][clmnN].setVal(cell_val);
    }
    emptyCellSelection();
    mRegularStepCount = mgr->mRegularStepCount;
}

void CellManager::clean()
{
    CellVal cell_val = EMPTY_CELL_VAL;
    for (int rowN = 0; rowN < BOARD_LEN; rowN++)
    for (int clmnN = 0; clmnN < BOARD_LEN; clmnN++)
    {
        cell_val = mCellMatrix[rowN][clmnN].getVal();
        if (cell_val != WHITE_CELL_VAL && cell_val != EMPTY_CELL_VAL)
        {
            mCellMatrix[rowN][clmnN].setVal(EMPTY_CELL_VAL);
        }
    }

    emptyCellSelection();
    resetRegularStepCount();
}

void CellManager::initGame(bool userBottomSide, CellMgrInitObj *initObj)
{
    if (initObj)
    {
        initCustomGame(initObj, userBottomSide);
        return;
    }

    clean();

    CellVal cell_val = EMPTY_CELL_VAL;
    for (int rowN = 0; rowN < BOARD_LEN; rowN++)
    for (int clmnN = 0; clmnN < BOARD_LEN; clmnN++)
    {
        // draw white cells
        if ((rowN % 2 == 0 && clmnN % 2 == 0) || (rowN % 2 == 1 && clmnN % 2 == 1))
        {
            cell_val = WHITE_CELL_VAL;
        }
        else
            switch(rowN)
            {
            case 0: case 1: case 2: // draw top populated cells
                cell_val = TOP_CELL_SOLD_VAL;
                break;
            case 5: case 6: case 7: // draw bottom populated cells
                cell_val = BOTTOM_CELL_SOLD_VAL;
                break;
            default:  // draw empty black cells
                cell_val = EMPTY_CELL_VAL;
                break;
            }

        mCellMatrix[rowN][clmnN].setVal(cell_val);
    }
}

void CellManager::initCustomGame(CellMgrInitObj *initObj, bool userBottomSide)
{
    clean();

    CellVal cell_val = EMPTY_CELL_VAL;
    int &kingsNumTop = (userBottomSide) ? initObj->compKingsNum : initObj->userKingsNum;
    int &piecesNumTop = (userBottomSide) ? initObj->compPiecesNum : initObj->userPiecesNum;

    for (int rowN = 0; rowN < 3; rowN++) // draw top populated cells
    for (int clmnN = 0; clmnN < BOARD_LEN; clmnN++)
    {
        if ((rowN % 2 == 0 && clmnN % 2 == 1) || (rowN % 2 == 1 && clmnN % 2 == 0))
        {
            if (kingsNumTop > 0)
            {
                cell_val = (userBottomSide) ? TOP_CELL_DAMKA_VAL : BOTTOM_CELL_DAMKA_VAL;
                --kingsNumTop;
            }
            else if (piecesNumTop > 0)
            {
                cell_val = (userBottomSide) ? TOP_CELL_SOLD_VAL : BOTTOM_CELL_SOLD_VAL;
                --piecesNumTop;
            }
            else
            {
                cell_val = EMPTY_CELL_VAL;
            }

            mCellMatrix[rowN][clmnN].setVal(cell_val);
        }

    }

    int &kingsNumBottom = (userBottomSide) ? initObj->userKingsNum : initObj->compKingsNum;
    int &piecesNumBottom = (userBottomSide) ? initObj->userPiecesNum : initObj->compPiecesNum;

    for (int rowN = BOARD_LEN - 1; rowN >= BOARD_LEN - 3; rowN--)
    for (int clmnN = 0; clmnN < BOARD_LEN; clmnN++)
    {
        if ((rowN % 2 == 0 && clmnN % 2 == 1) || (rowN % 2 == 1 && clmnN % 2 == 0))
        {
            if (kingsNumBottom > 0)
            {
                cell_val = (userBottomSide) ? BOTTOM_CELL_DAMKA_VAL : TOP_CELL_DAMKA_VAL;
                --kingsNumBottom;
            }
            else if (piecesNumBottom > 0)
            {
                cell_val = (userBottomSide) ? BOTTOM_CELL_SOLD_VAL : TOP_CELL_SOLD_VAL;
                --piecesNumBottom;
            }
            else
            {
                cell_val = EMPTY_CELL_VAL;
            }

            mCellMatrix[rowN][clmnN].setVal(cell_val);
        }
    }
}

// move the cell content from current selected to toPos
void CellManager::moveSelectedCell(const Position& toPos)
{
    moveCell(mCurrentSelectedPos, toPos);
}

int CellManager::soldNumInRow(int rowN, bool userBottomSide)
{
    CellVal cell_val = userBottomSide ? BOTTOM_CELL_SOLD_VAL : TOP_CELL_SOLD_VAL;
    int count = 0;
    for (int clmnN = 0; clmnN < BOARD_LEN; clmnN++)
    {
        if (mCellMatrix[rowN][clmnN].getVal() == cell_val) ++count;
    }
    return count;
}

void CellManager::moveCellData(const Position& fromPos, const Position& toPos)
{
    CellVal val = mCellMatrix[fromPos.y()][fromPos.x()].getVal();
    mCellMatrix[toPos.y()][toPos.x()].setVal(val);
    mCellMatrix[fromPos.y()][fromPos.x()].setVal(EMPTY_CELL_VAL);
}

void CellManager::moveCell(const Position& fromPos, const Position& toPos)
{
    cout<<"Move from x="<<fromPos.x()<<","<<fromPos.y()<<" to x="<< toPos.x()<<",y="<<toPos.y() << endl;
    Cell& fromCell = mCellMatrix[fromPos.y()][fromPos.x()];
    mCellMatrix[toPos.y()][toPos.x()] = fromCell;
    fromCell.empty();
}

void CellManager::removeCell(const Position& pos, bool full_remove)
{
    Cell& cell = mCellMatrix[pos.y()][pos.x()];
    cell.empty(full_remove);
}

bool CellManager::isLegalPos(const Position& pos)
{
    return (pos.x() >= 0 && pos.x() < BOARD_LEN && pos.y() >= 0 && pos.y() < BOARD_LEN);
}

bool CellManager::isEmptyCell(const Position& pos)
{
    if (!isLegalPos(pos)) return false;
    Cell& posCell = mCellMatrix[pos.y()][pos.x()];
    return (posCell.getVal() == EMPTY_CELL_VAL);
}

bool CellManager::isSold(const Position& pos, bool userBottomSide)
{
    CellVal val = mCellMatrix[pos.y()][pos.x()].getVal();
    CellVal sold = (userBottomSide) ? BOTTOM_CELL_SOLD_VAL : TOP_CELL_SOLD_VAL ;
    return (val == sold);
}

bool CellManager::isDamka(const Position& pos, bool userBottomSide)
{
    CellVal val = mCellMatrix[pos.y()][pos.x()].getVal();
    CellVal damka = (userBottomSide) ? BOTTOM_CELL_DAMKA_VAL : TOP_CELL_DAMKA_VAL ;
    return (val == damka);
}

bool CellManager::isFishkaPos(const Position& pos, bool userBottomSide)
{
    return (isSold(pos, userBottomSide) || isDamka(pos, userBottomSide));
}

bool CellManager::isUserSideFishka(const Position& pos, bool userBottomSide)
{
    CellVal val = mCellMatrix[pos.y()][pos.x()].getVal();
    CellVal sold = (userBottomSide) ? BOTTOM_CELL_SOLD_VAL : TOP_CELL_SOLD_VAL ;
    CellVal damka = (userBottomSide) ? BOTTOM_CELL_DAMKA_VAL : TOP_CELL_DAMKA_VAL ;
    return (val == sold || val == damka);
}

bool CellManager::convertToDamkaIfNeed(const Position& pos)
{
    if (reachedLastLine(pos)) // if last line
    {
        // convert to damka
        CellVal currVal = mCellMatrix[pos.y()][pos.x()].getVal();
        CellVal damkaVal = (currVal == BOTTOM_CELL_SOLD_VAL) ? BOTTOM_CELL_DAMKA_VAL : TOP_CELL_DAMKA_VAL;
        mCellMatrix[pos.y()][pos.x()].setVal(damkaVal);
        return true;
    }
    return false;
}

void CellManager::revertFromDamka(const Position& pos)
{
    CellVal currVal = mCellMatrix[pos.y()][pos.x()].getVal();
    CellVal soldVal = (currVal == BOTTOM_CELL_DAMKA_VAL) ? BOTTOM_CELL_SOLD_VAL : TOP_CELL_SOLD_VAL;
    mCellMatrix[pos.y()][pos.x()].setVal(soldVal);
}

// verify is soldier reached last line and should be converted to damka
bool CellManager::reachedLastLine(const Position& pos)
{
    CellVal cel_val = getCellVal(pos);

    if (pos.y() == 0 && cel_val == BOTTOM_CELL_SOLD_VAL) return true;
    if (pos.y() == BOARD_LEN - 1 && cel_val == TOP_CELL_SOLD_VAL) return true;

    return false;
}

void CellManager::makeCalculatingStep(BaseStep& step)
{
    moveCellData(step.fromPos(), step.toPos());
    for (const FishkaPosition& pos : step.arrToEat())
    {
        removeCell(pos, false);
    }

    if (convertToDamkaIfNeed(step.toPos()))
    {
        step.setConvertDoDamka();
    }
}

void CellManager::revertStep(const BaseStep& step)
{
    moveCellData(step.toPos(), step.fromPos());

    for (const FishkaPosition& eatPos : step.arrToEat())
    {
        setCellVal(eatPos, eatPos.getFishkaVal());
    }

    if (step.shouldConvertToDamka())
    {
        revertFromDamka(step.fromPos());
    }
}

CellManager* CellManager::clone()
{
    CellManager* clonedManager = new CellManager(this);
    return clonedManager;
}

//////////////////////////////////////////////////////////////////////////
///
/////////////////////////////////////////////////////////////////////////////

void Cell::empty(bool full_empty)
{
    if (mVal != WHITE_CELL_VAL)
        mVal = EMPTY_CELL_VAL;

    if (full_empty)
    {
    // don't release here the pixmap, its object can be used by someone else
        mPixmap = nullptr;
    }
}

bool operator==(const Position& pos1, const Position& pos2)
{
    return (pos1.x() == pos2.x() && pos1.y() == pos2.y());
}

const Cell& Cell::operator=(const Cell& cell)
{
    mVal = cell.mVal;
    mPixmap = cell.mPixmap;
    return *this;
}

