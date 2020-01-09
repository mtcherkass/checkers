#include "utility.h"
#include "statistic.h"
#include "cell_manager.h"

Statistic::Statistic(CellManager* cellMgr, bool userBottomside) :
    mCellMgr(cellMgr), mUserBottomSide(userBottomside),
    mMySoldNum(0), mMyDamkaNum(0), mOppSoldNum(0), mOppDamkaNum(0)
{
    for (int i = 0; i < BOARD_LEN; i++) mMyRowNumArr[i] = 0;
    for (int i = 0; i < BOARD_LEN; i++) mOppRowNumArr[i] = 0;

    calculateFishkaCount();
    calculateSoldsInRowArr(mMyRowNumArr, mUserBottomSide);
    calculateSoldsInRowArr(mOppRowNumArr, !mUserBottomSide);
}

void Statistic::calculateSoldsInRowArr(int* arr, bool userBottomSide)
{
    for (int i = 0; i < BOARD_LEN; i++)
    {
        arr[i] = mCellMgr->soldNumInRow(i, userBottomSide);
    }
}

void Statistic::calculateFishkaCount()
{
    Position pos;

    for (int rowN = 0; rowN < BOARD_LEN; rowN++)
    for (int clmnN = 0; clmnN < BOARD_LEN; clmnN++)
    {
        pos.set(clmnN, rowN);
        CellVal val = mCellMgr->getCellVal(pos);

        if (mUserBottomSide)
        {
            switch(val)
            {
            case BOTTOM_CELL_SOLD_VAL: ++mMySoldNum; break;
            case BOTTOM_CELL_DAMKA_VAL: ++mMyDamkaNum; break;
            case TOP_CELL_SOLD_VAL: ++mOppSoldNum; break;
            case TOP_CELL_DAMKA_VAL: ++ mOppDamkaNum; break;
            default: break;
            }
        }
        else
        {
            switch(val)
            {
            case BOTTOM_CELL_SOLD_VAL: ++mOppSoldNum; break;
            case BOTTOM_CELL_DAMKA_VAL: ++mOppDamkaNum; break;
            case TOP_CELL_SOLD_VAL: ++mMySoldNum; break;
            case TOP_CELL_DAMKA_VAL: ++ mMyDamkaNum; break;
            default: break;
            }
        }
    }
}

int Statistic::getMySoldsInRow(int rowN)
{
    if (rowN < 0 || rowN >= BOARD_LEN)
    {
        string msg("getMySoldsInRow: bad index ", rowN);
        showErrorMsg(msg.c_str());
        return 0;
    }

    return mMyRowNumArr[rowN];
}

int Statistic::getOppSoldsInRow(int rowN)
{
    if (rowN < 0 || rowN >= BOARD_LEN)
    {
        string msg("getOppSoldsInRow: bad index ", rowN);
        showErrorMsg(msg.c_str());
        return 0;
    }

    return mOppRowNumArr[rowN];
}
