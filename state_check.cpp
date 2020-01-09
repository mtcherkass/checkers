#include "state_check.h"
#include "statistic.h"
#include "utility.h"
#include <iostream>

//---------------------------------------------------------------------
// verify is it regular step for soldier
bool SoldCheck::isRegularStep(const Position& currentSelectedPos,
                               const Position& newSelectedCellPos, bool userBottomSide)
{
    int yDelta = (userBottomSide) ? 1 : -1;

    if ((currentSelectedPos.x() == newSelectedCellPos.x() + 1) &&
            currentSelectedPos.y() == newSelectedCellPos.y() + yDelta) return true;
    if ((currentSelectedPos.x() == newSelectedCellPos.x() - 1) &&
            currentSelectedPos.y() == newSelectedCellPos.y() + yDelta) return true;

    return false;
}

//---------------------------------------------------------------------
//---------------------------------------------------------------------
// collects steps for damka in one direction
void DamkaCheck::collectStepsForDirection(int xDir, int yDir, const Position& pos,
                                          vector<Position>& possibleStepPosArr)
{
    Position checkedPos = pos;
    while (true)
    {
        checkedPos.changeBy(xDir, yDir);
        if (!mCheckObj->getManager()->isEmptyCell(checkedPos)) break;
        possibleStepPosArr.push_back(checkedPos);
    }
}

// verify is it regular step for soldier
bool DamkaCheck::isRegularStep(const Position& currentSelectedPos,
                          const Position& newSelectedCellPos, bool)
{
    vector<Position> possibleStepPosArr;

    collectStepsForDirection( 1,  1, currentSelectedPos, possibleStepPosArr);
    collectStepsForDirection(-1,  1, currentSelectedPos, possibleStepPosArr);
    collectStepsForDirection( 1, -1, currentSelectedPos, possibleStepPosArr);
    collectStepsForDirection(-1, -1, currentSelectedPos, possibleStepPosArr);

    for (Position& pos : possibleStepPosArr)
    {
        if (pos == newSelectedCellPos) return true;
    }
    return false;
}

//---------------------------------------------------------------------
//---------------------------------------------------------------------
// find array of best eating steps by analizing array of all possible beat steps and
// providing the array with maximum beated fishkas
bool StateCheck::findBestEatingArray(bool userBottomSide, vector<BaseStep>& fishkaArrToRemove)
{
    bool hasEatingStep = false;
    vector<BaseStep> eatingStepsArr[FISHKA_COUNT];

    findAllEatingSteps(userBottomSide, eatingStepsArr);
    int highestIdx = -1;

    // find highest index (eating maximum fiskas)
    for (int i = 0; i < FISHKA_COUNT; i++)
    {
        if (eatingStepsArr[i].size() == 0) { highestIdx = i - 1; break; }
    }

    if (highestIdx > -1 && eatingStepsArr[highestIdx].size() > 0)
    {
        hasEatingStep = true;
        for (int unsigned i = 0; i < eatingStepsArr[highestIdx].size(); i++)
        {
            fishkaArrToRemove.push_back(eatingStepsArr[highestIdx].at(i));
        }
    }

    return hasEatingStep;
}

// analyze input step for eating. It has 4 return states:
// EATING_OK, EATING_NOT_FOUND, EAT_NOT_MAXIMUM, DOES_NOT_EAT_BUT_CAN
EatingStatus StateCheck::verifyEatingStep(const Position& currentSelectedPos,
                              const Position& newSelectedCellPos,
                              bool userBottomSide, vector<FishkaPosition>& eatingFishkaArr)
{
    // eatingFishkaArr[i] -> is array of positions for eating fishkas
    vector<BaseStep> eatingStepsArr[FISHKA_COUNT];

    findAllEatingSteps(userBottomSide, eatingStepsArr);
    int highestIdx = -1;

    // find highest index (eating maximum fishkas)
    for (int i = 0; i < FISHKA_COUNT; i++)
    {
        if (eatingStepsArr[i].size() == 0) { highestIdx = i - 1; break; }
    }

    if (highestIdx >= 0)
    {
        // check if the step is maximum eating step
        for (unsigned int counter = 0; counter < eatingStepsArr[highestIdx].size(); counter++)
        {
            BaseStep& eatingStep = eatingStepsArr[highestIdx].at(counter);
            if (eatingStep.fromPos() == currentSelectedPos &&
                    eatingStep.toPos() == newSelectedCellPos)
            {
                for (unsigned int i = 0; i < eatingStep.arrToEat().size(); i++)
                {
                    eatingFishkaArr.push_back(eatingStep.arrToEat().at(i));
                }

                return EATING_OK;
            }
        }

        // check if the step is eating step (but not  maximum)
        for (int i = 0; i < highestIdx; i++)
        {
            for (unsigned int counter = 0; counter < eatingStepsArr[i].size(); counter++)
            {
                BaseStep& eatingStep = eatingStepsArr[i].at(counter);
                if (eatingStep.fromPos() == currentSelectedPos &&
                        eatingStep.toPos() == newSelectedCellPos) return EAT_NOT_MAXIMUM;
            }
        }
    }
    else
    {
        return EATING_NOT_FOUND;
    }

    return DOES_NOT_EAT_BUT_CAN;
}

// check if current position cell is legal to be selected for moving
bool StateCheck::isCellLegalForSelect(const Position& pos, bool userBottomSide)
{
    return (mCellMgr->isFishkaPos(pos, userBottomSide));
}

// analize input step for corectness. Possible return values:
// ILLEGAL_STEP, MUST_BEAT_ERROR_STEP, HAVE_BETTER_BEAT_STEP, OK_STEP
StepErrorType StateCheck::stepVerify(const Position& currentSelectedPos,
                          const Position& newSelectedCellPos, bool userBottomSide,
                          vector<FishkaPosition>& fishkaArrToRemove)
{
    CellVal cell_val = mCellMgr->getCellVal(newSelectedCellPos);
    if (cell_val != EMPTY_CELL_VAL) return ILLEGAL_STEP;

    StepErrorType errType = OK_STEP;
    bool isSold = mCellMgr->isSold(currentSelectedPos, userBottomSide);
    CheckIntf *checkObj = nullptr;

    if (isSold)
    {
        checkObj = new SoldCheck();
    }
    else
    {
        checkObj = new DamkaCheck(this);
    }

    EatingStatus status = verifyEatingStep(currentSelectedPos,
                newSelectedCellPos, userBottomSide, fishkaArrToRemove);

    switch (status)
    {
    case EATING_OK:             break;
    case EAT_NOT_MAXIMUM:       errType = HAVE_BETTER_BEAT_STEP; break;
    case DOES_NOT_EAT_BUT_CAN:
        if (checkObj->isRegularStep(currentSelectedPos, newSelectedCellPos, userBottomSide))
            errType = MUST_BEAT_ERROR_STEP;
        else
            errType =  ILLEGAL_STEP;
        break;
    case EATING_NOT_FOUND:
        if (!checkObj->isRegularStep(currentSelectedPos, newSelectedCellPos, userBottomSide))
            errType = ILLEGAL_STEP;
        break;
    }

    delete checkObj;
    return errType;
}

// verify is input position coordinates are legal
bool StateCheck::isLegalPos(const Position& pos)
{
    return (pos.x() >= 0 && pos.y() >= 0 && pos.x() < BOARD_LEN && pos.y() < BOARD_LEN);
}

// find all possible eating steps
void StateCheck::findAllEatingSteps(bool userBottomSide, vector<BaseStep> eatingStepsArr[])
{
    Position emptyPos;
    Position fishkaPos;
    for (int clmN = 0; clmN < BOARD_LEN; clmN++)
    {
        for (int rowN = 0; rowN < BOARD_LEN; rowN++)
        {
            fishkaPos.set(clmN, rowN);
            BaseStep step(fishkaPos, fishkaPos);

            if (mCellMgr->isSold(fishkaPos, userBottomSide)) //if it is soldier fishka cell
            {
                // for soldier use maxLen = 2
                findFishkaEatingSteps(step, 2, NO_DIRECTION,
                                      userBottomSide, eatingStepsArr, 0, true);
            }

            if (mCellMgr->isDamka(fishkaPos, userBottomSide)) //if it is damka fishka cell
            {
                // for damka use maxLen = BOARD_LEN
                findFishkaEatingSteps(step, BOARD_LEN, NO_DIRECTION,
                                      userBottomSide, eatingStepsArr, 0, false);
            }
        }
    }
}

// find direction which is opposite to input direction
int StateCheck::getOppositeDirection(int direction)
{
    switch (direction)
    {
    case RIGHT_FORWARD_DIRECTION:   return LEFT_BACK_DIRECTION;
    case LEFT_BACK_DIRECTION:       return RIGHT_FORWARD_DIRECTION;
    case RIGHT_BACK_DIRECTION:      return LEFT_FORWARD_DIRECTION;
    case LEFT_FORWARD_DIRECTION:    return RIGHT_BACK_DIRECTION;
    }

    return DIRECTION_NUM;
}

// find position of opposite fishka in given chain defined by pos + direction + interval
bool StateCheck::findOppositePosInChain(const Position& pos, int interval,
                                        int direction, bool userBottomSide,
                                        FishkaPosition &oppositerPos)
{
    Position checkedPos = pos;
    int foundNum = 0;
    for (int i = 0; i < interval; i++)
    {
        switch (direction)
        {
        case RIGHT_FORWARD_DIRECTION:
            checkedPos.changeBy(1, userBottomSide ? -1 : 1);
            break;
        case LEFT_FORWARD_DIRECTION:
            checkedPos.changeBy(-1, userBottomSide ? -1 : 1);
            break;
        case RIGHT_BACK_DIRECTION:
            checkedPos.changeBy(1, userBottomSide ? 1 : -1);
            break;
        case LEFT_BACK_DIRECTION:
            checkedPos.changeBy(-1, userBottomSide ? 1 : -1);
            break;
        }

         // if found user fishka, break the checking
        if (mCellMgr->isUserSideFishka(checkedPos, userBottomSide)) break;

        // if found opposite fishka, count it
        bool oppBottomSide = userBottomSide ? false : true;
        if (mCellMgr->isUserSideFishka(checkedPos, oppBottomSide))
        {
            oppositerPos.set(checkedPos);
            CellVal val = EMPTY_CELL_VAL;
            if (mCellMgr->isSold(checkedPos, oppBottomSide))
            {
                val = (oppBottomSide) ? BOTTOM_CELL_SOLD_VAL : TOP_CELL_SOLD_VAL;
            }
            else
            {
                val = (oppBottomSide) ? BOTTOM_CELL_DAMKA_VAL : TOP_CELL_DAMKA_VAL;
            }
            oppositerPos.setFishkaVal(val);
            ++foundNum;
        }
    }

    return (foundNum == 1);
}

// Find available eating steps for given cell .
// max_step - 2 for soldier and 8 for damka
// prev_direction - used to define that we are not going back to same place we come from,
//                  and avoiding infinite recursion
// stepNum - number of eating jumps
// only_forward_dir - false for regular soldier
void StateCheck::findFishkaEatingSteps(const BaseStep& step,
                                    int max_step, int prev_direction,
                                    bool userBottomSide,
                                    vector<BaseStep> eatingStepsArr[],
                                    int stepNum,
                                    bool only_forward_dir)
{
    for (int direction = 0; direction < DIRECTION_NUM; direction++)
    {
        if (only_forward_dir &&
                (direction == RIGHT_BACK_DIRECTION || direction == LEFT_BACK_DIRECTION))
            continue;

        if (prev_direction == getOppositeDirection(direction))
            continue;

        for (int i = 1; i < max_step; i++)
        {
            if (!findEatingSteps(step, i + 1, stepNum, max_step, direction, userBottomSide, eatingStepsArr)) break;
        }
    }
}

// find available eating steps for given cell having damka fishka in given direction
bool StateCheck::findEatingSteps(const BaseStep& step, int stepVal,
                                 int stepNum, int max_step, int direction, bool userBottomSide,
                                 vector<BaseStep> eatingStepsArr[])
{
    bool ret_val = true;
    const Position& pos = step.toPos();
    FishkaPosition oppositerPos(pos);
    Position afterEatPos(pos);

    if (stepNum == 12)
    {
        showErrorMsg("findEatingSteps: stack overflow");
        return false;
    }

    switch (direction)
    {
    case RIGHT_FORWARD_DIRECTION:
        afterEatPos.changeBy(stepVal, userBottomSide ? -stepVal : stepVal);
        break;
    case LEFT_FORWARD_DIRECTION:
        afterEatPos.changeBy(-stepVal, userBottomSide ? -stepVal : stepVal);
        break;
    case RIGHT_BACK_DIRECTION:
        afterEatPos.changeBy(stepVal, userBottomSide ?  stepVal : -stepVal);
        break;
    case LEFT_BACK_DIRECTION:
        afterEatPos.changeBy(-stepVal, userBottomSide ? stepVal : -stepVal);
        break;
    }

    if (!mCellMgr->isLegalPos(afterEatPos) ||
         mCellMgr->isUserSideFishka(afterEatPos, userBottomSide))
    {
        ret_val = false;
    }
    else if (findOppositePosInChain(pos, stepVal - 1, direction,
                               userBottomSide, oppositerPos) &&
            mCellMgr->isEmptyCell(afterEatPos))
    {
        BaseStep newStep(step);
        newStep.setPos(step.fromPos(), afterEatPos);

        // in order to prevent infinite loop of eating
        // we must check is the eating position already was taken
        if (!isPositionExist(newStep.arrToEatRef(), oppositerPos))
        {
            newStep.arrToEatRef().push_back(oppositerPos);

            eatingStepsArr[stepNum].push_back(newStep);

            findFishkaEatingSteps(newStep, max_step, direction,
                                  userBottomSide, eatingStepsArr, stepNum + 1, false);
        }
    }

    return ret_val;
}

bool StateCheck::isPositionExist(const vector<FishkaPosition>& arr, const FishkaPosition& pos)
{
    for (const FishkaPosition& arrPos : arr)
    {
        if (arrPos == pos) return true;
    }
    return false;
}

// check for tie (no winner) by remaining fishkas
bool StateCheck::isTie()
{
    Statistic stat1(mCellMgr, true);
    Statistic stat2(mCellMgr, false);

    int damkaNum1 = stat1.getMyDamkaNum();
    int damkaNum2 = stat2.getMyDamkaNum();
    int soldNum1 = stat1.getMySoldNum();
    int soldNum2 = stat2.getMySoldNum();

    if ((soldNum1 == 0 && (damkaNum1 == 1 || damkaNum1 == 2)) || (soldNum1 == 1 && damkaNum1 == 1))
        if ((soldNum2 == 0 && (damkaNum2 == 1 || damkaNum2 == 2)) || (soldNum2 == 1 && damkaNum2 == 1))
            return true;

    return false;
}
