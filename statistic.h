// statistic.h
#ifndef STATISTIC_H
#define STATISTIC_H

#include "cell_manager.h"

class Statistic
{
    CellManager* mCellMgr;
    bool mUserBottomSide;

    int mMySoldNum;
    int mMyDamkaNum;
    int mOppSoldNum;
    int mOppDamkaNum;

    int mMyRowNumArr[BOARD_LEN];
    int mOppRowNumArr[BOARD_LEN];

    void calculateFishkaCount();
    void calculateSoldsInRowArr(int* arr, bool userBottomSide);

public:
    Statistic(CellManager* cellMgr, bool userBottomside);

    int getMySoldNum() { return mMySoldNum; }
    int getMyDamkaNum() { return mMyDamkaNum; }
    int getOppSoldNum() { return mOppSoldNum; }
    int getOppDamkaNum() { return mOppDamkaNum; }
    int getMySoldsInRow(int rowN);
    int getOppSoldsInRow(int rowN);
    bool isMyEnd() { return (mMySoldNum == 0 && mMyDamkaNum == 0); }
    bool isOppEnd() { return (mOppSoldNum == 0 && mOppDamkaNum == 0); }
};

#endif // STATISTIC_H
