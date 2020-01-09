#include "calculate_step.h"
#include "utility.h"

#define PRINT_STEP(step) " from(X="<<step.fromPos.x()<<",Y="<<step.fromPos.y()<< ")  to(X="<<step.toPos.x()<<",Y="<<step.toPos.y()
#define SOLD_POINTS 20
#define DAMKA_POINTS 70

class TestStep
{
public:
    TestStep(CellManager* cellMgr, BaseStep& step) : mCellMgr(cellMgr), mStep(step)
    {
        mCellMgr->makeCalculatingStep(mStep);
    }

    ~TestStep()
    {
        mCellMgr->revertStep(mStep);
    }

private:
    CellManager* mCellMgr;
    BaseStep& mStep;
};

// collect all available legal steps
// if exist eating steps, only best eating steps will be collected
bool CalculateStep::collectAvailableSteps(StateCheck* stateChk, bool userBottomSide,
                                      int level, vector<BaseStep>& allStepsArr)
{
    // search for eating steps
    stateChk->findBestEatingArray(userBottomSide, allStepsArr);

    // if and only if no eating steps, we should collect the regular steps
    if (allStepsArr.size() == 0)
    {
        collectRegularSteps(userBottomSide, level, allStepsArr);
    }

    return (allStepsArr.size() > 0);
}

bool CalculateStep::hasAvailableSteps(StateCheck* stateChk, bool userBottomSide)
{
    vector<BaseStep> availableStepsArr;
    return collectAvailableSteps(stateChk, userBottomSide, 0, availableStepsArr);
}

void CalculateStep::collectRegularSteps(bool userBottomSide,
                                        int level, vector<BaseStep>& stepsArr)
{
    Position pos;
    for (int rowN = 0; rowN < BOARD_LEN; rowN++)
    for (int clmnN = 0; clmnN < BOARD_LEN; clmnN++)
    {
        pos.set(clmnN, rowN);

        if (mCellMgr->isFishkaPos(pos, userBottomSide))
        {
            generateAllRegularSteps(pos, userBottomSide, level, stepsArr);
        }
    }
}

// main calculating func
// calculate array of steps with best estimate
int CalculateStep::calculateBestStep(StateCheck* stateChk, bool userBottomSide,
                                     int deep, int level, BaseStep& bestStep)
{
    if (deep == 0) mLogFile.open ("log.txt", ios::out | ios::trunc);

    vector<BaseStep> availableStepsArr;

    bool hasSteps = collectAvailableSteps(stateChk, userBottomSide, level, availableStepsArr);
    if (!hasSteps) return LOSE_ESTIMATE;

    vector<BaseStep> bestStepArr;
    int stepEstimate = 0;
    int bestStepEstimate = SMALLEST_ESTIMATE * 2;

    for (BaseStep& availableStep : availableStepsArr)
    {
        stepEstimate = calculateEstimateForStep(stateChk, userBottomSide,
                                                deep, level, availableStep);
        if (stepEstimate > bestStepEstimate)
        {
            bestStepEstimate = stepEstimate;
            bestStepArr.clear();
            bestStepArr.push_back(availableStep);
        }
        else if (stepEstimate == bestStepEstimate)
        {
            bestStepArr.push_back(availableStep);
        }
    }

    if (deep == 0)
    {
        mLogFile << "Best estimate is " << bestStepEstimate << endl;
        for (BaseStep& stp : bestStepArr)
        {
            mLogFile << "Best step : " << getStepLogString(stp) << endl;
        }
    }

    unsigned int sizeOfBestArr = bestStepArr.size();
    bestStep = bestStepArr[rand() % sizeOfBestArr];

    if (deep == 0)
    {
        logOutBoard();
        mLogFile.close();
    }
    return bestStepEstimate;
}


// calculate estimate for giving step
int CalculateStep::calculateEstimateForStep(StateCheck* stateChk, bool userBottomSide,
                                            int deep, int level, BaseStep& estimatedStep)
{
    if (deep == 0)
        mLogFile << "Calculating for step" << getStepLogString(estimatedStep) << endl;

    int worstEstimate = BIGGEST_ESTIMATE;
    int estimate = 0;
    Statistic sourceStat(mCellMgr, userBottomSide);

    TestStep testStep(mCellMgr, estimatedStep);

    vector<BaseStep> availableStepsArr;

    collectAvailableSteps(stateChk, !userBottomSide, level, availableStepsArr);
    if (availableStepsArr.empty())
        return WIN_ESTIMATE;

    for (BaseStep& availableStep : availableStepsArr)
    {
        TestStep testStepInner(mCellMgr, availableStep);

        int bestComputerSecondEstimate = 0;
        if (deep == level)
        {
            bestComputerSecondEstimate = 0;
        }
        else // not final
        {
            BaseStep bestComputerSecondStep;
            bestComputerSecondEstimate = calculateBestStep(stateChk, userBottomSide,
                                                           deep + 1, level, bestComputerSecondStep);
            // no steps found, give it very low estimate
            if (bestComputerSecondEstimate == SMALLEST_ESTIMATE)
                bestComputerSecondEstimate = VERY_LOW_ESTIMATE;
        }


        Statistic destStat(mCellMgr, userBottomSide);
        estimate = getEstimateByStatistics(&sourceStat, &destStat, userBottomSide, deep, level);
        estimate += bestComputerSecondEstimate;
        if (estimate < worstEstimate)
        {
            worstEstimate = estimate;
        }

        if (deep == 0)
        {
            mLogFile << "Estimate for step" << getStepLogString(availableStep)  <<
                         "is: " << estimate << ", answer estimate (" <<
                        bestComputerSecondEstimate << ")" << endl;
        }
    }

    if (deep == 0)
        mLogFile << "Step estimate is: " << worstEstimate << endl << "******" << endl;

    return worstEstimate;
}

const char* CalculateStep::getStepLogString(const BaseStep& step)
{
    static char sLogBuf[100];
    sprintf(sLogBuf, " from(X=%d, Y=%d) , to(X=%d, Y=%d) ",
            step.fromPos().x(), step.fromPos().y(), step.toPos().x(), step.toPos().y());

    return &(sLogBuf[0]);
}

inline int CalculateStep::calc(int my, int opp, int estimateDelta)
{
    return (my - opp) * estimateDelta;
}

// generate estimate by given source and destination statistics
int CalculateStep::getEstimateByStatistics(Statistic* source, Statistic* dest,
                                           bool userBottomSide, int deep, int level)
{
    int estimate = 0;
    int mySoldPoints = SOLD_POINTS;
    int myDamkaPoints = DAMKA_POINTS;
    int oppSoldPoints = -SOLD_POINTS;
    int oppDamkaPoints = -DAMKA_POINTS;

    if (dest->isMyEnd())
        return LOSE_ESTIMATE;

    if (dest->isOppEnd())
        return WIN_ESTIMATE;

    if (level == 2)
    {
        // When my position is better, I want to have less fishkas on the board so
        // eating together (me & oppositor) is good for me.
        // On the other hand if my state is not good I don't want any eatings.
        mySoldPoints += hasIbetterStateAdjust(source, mySoldPoints);
        myDamkaPoints += hasIbetterStateAdjust(source, myDamkaPoints);

        oppDamkaPoints += endGameOppDamkaAdjust(source, oppDamkaPoints);
        myDamkaPoints += endGameMyDamkaAdjust(source, myDamkaPoints);
    }

    estimate += calc(dest->getMySoldNum(), source->getMySoldNum(), mySoldPoints);
    estimate += calc(dest->getOppSoldNum(), source->getOppSoldNum(), oppSoldPoints);
    estimate += calc(dest->getMyDamkaNum(), source->getMyDamkaNum(), myDamkaPoints);
    estimate += calc(dest->getOppDamkaNum(), source->getOppDamkaNum(), oppDamkaPoints);

    if (level > 0)
    {
        updateEstimateForSoldInRow(source, dest, userBottomSide, estimate);
    }

    return estimate - estimate * deep / 4;
}

void CalculateStep::updateEstimateForSoldInRow(Statistic* source, Statistic* dest,
                                               bool userBottomSide, int& estimate)
{
    // define rows estimates.
    // First has no points and last has no meaning as it should be converted to damka
    int bottomEstimateArr[BOARD_LEN] = {0, 13, 8, 5, 3, 2, 1, 0};
    int topEstimateArr[BOARD_LEN] = {0, 1, 2, 3, 5, 8, 13, 0};

    int* myEstimateArr = (userBottomSide) ? bottomEstimateArr : topEstimateArr;
    int* oppEstimateArr = (userBottomSide) ? topEstimateArr : bottomEstimateArr;

    for (int unsigned i = 0; i < BOARD_LEN; i++)
    {
        if (i == 0 || i == BOARD_LEN - 1) continue;
        estimate += calc(dest->getMySoldsInRow(i),
                         source->getMySoldsInRow(i), myEstimateArr[i]);
    }
    for (int unsigned i = 0; i < BOARD_LEN; i++)
    {
        if (i == 0 || i == BOARD_LEN - 1) continue;
        estimate += calc(dest->getOppSoldsInRow(i),
                         source->getOppSoldsInRow(i), -oppEstimateArr[i]);
    }
}

// generate (if possible) regular step and add it to the bestStepsArr
// return true if step was created, false if it is not legal to generate such step
bool CalculateStep::generateRegularStep(const Position& currPos, bool userBottomSide,
                                        int direction, int stepVal,
                                        vector<BaseStep>& stepsArr)
{
    bool ret_val = false;
    int xDelta = 0, yDelta = 0;
    int vertDirectionCoeff = (userBottomSide ? -1 : 1);

    switch (direction)
    {
    case RIGHT_FORWARD_DIRECTION:
        xDelta = stepVal; yDelta = stepVal * vertDirectionCoeff; break;
    case LEFT_FORWARD_DIRECTION:
        xDelta = -stepVal; yDelta = stepVal * vertDirectionCoeff; break;
    case RIGHT_BACK_DIRECTION:
        xDelta = stepVal; yDelta = -stepVal * vertDirectionCoeff; break;
    case LEFT_BACK_DIRECTION:
        xDelta = -stepVal; yDelta = -stepVal * vertDirectionCoeff; break;
    }

    Position newPos(currPos.x() + xDelta, currPos.y() + yDelta);
    if (mCellMgr->isEmptyCell(newPos))
    {
        BaseStep stp(currPos, newPos);
        stepsArr.push_back(stp);
        ret_val = true;
    }

    return ret_val;
}


void CalculateStep::generateAllRegularSteps(const Position& pos, bool userBottomSide,
                                          int level, vector<BaseStep>& bestStepsArr)
{
    Position stepPos;

    if (mCellMgr->isSold(pos, userBottomSide))
    {
        generateRegularStep(pos, userBottomSide, RIGHT_FORWARD_DIRECTION, 1, bestStepsArr);
        generateRegularStep(pos, userBottomSide, LEFT_FORWARD_DIRECTION, 1, bestStepsArr);
    }
    else if (mCellMgr->isDamka(pos, userBottomSide))
    {
        vector<BaseStep> oneDirStepsArr(8);
        for (int direction = 0; direction < DIRECTION_NUM; direction++)
        {
            oneDirStepsArr.clear();
            for (int stp = 1; stp < BOARD_LEN; stp++)
            {
                if (!generateRegularStep(pos, userBottomSide, direction, stp, oneDirStepsArr))
                    break;
            }
            if (level == 2 && oneDirStepsArr.size() > 3)
            {
                BaseStep oneDirSmallStepsArr[3];
                findSmallStepsArr(oneDirStepsArr, pos, oneDirSmallStepsArr);
                for (BaseStep& stp : oneDirSmallStepsArr) bestStepsArr.push_back(stp);
            }
            else
            {
                for (BaseStep& stp : oneDirStepsArr) bestStepsArr.push_back(stp);
            }
        }
    }
}

void CalculateStep::findSmallStepsArr(const vector<BaseStep>& oneDirStepsArr,
                                      const Position& pos,
                                      BaseStep oneDirSmallStepsArr[3])
{
    int size = oneDirStepsArr.size();
    oneDirSmallStepsArr[0] = oneDirStepsArr[0];
    int middleIdx = 1 +((pos.x() + pos.y()) % (size - 2));
    oneDirSmallStepsArr[1] = oneDirStepsArr[middleIdx];
    oneDirSmallStepsArr[2] = oneDirStepsArr[size - 1];
}

void CalculateStep::logOutBoard()
{
    Position pos;
    string val;

    for (int rowN = 0; rowN < BOARD_LEN; rowN++)
    {
        for (int clmnN = 0; clmnN < BOARD_LEN; clmnN++)
        {
            val = "";
            pos.set(clmnN, rowN);
            switch (mCellMgr->getCellVal(pos))
            {
            case WHITE_CELL_VAL: val = "  "; break;
            case EMPTY_CELL_VAL: val = "--"; break;
            case TOP_CELL_SOLD_VAL: val = "TS"; break;
            case TOP_CELL_DAMKA_VAL: val = "TD"; break;
            case BOTTOM_CELL_SOLD_VAL: val = "BS"; break;
            case BOTTOM_CELL_DAMKA_VAL: val = "BD"; break;
            }

            val += (mCellMgr->getCellPixmap(pos) == nullptr) ? " " : "I";
            mLogFile << val.c_str() << "  ";
        }
        mLogFile << endl;
    }
}

int CalculateStep::hasIbetterStateAdjust(Statistic* statistic, int sourceEstimate)
{
    int mySold = statistic->getMySoldNum();
    int myDamka = statistic->getMyDamkaNum();
    int oppSold = statistic->getOppSoldNum();
    int oppDamka = statistic->getOppDamkaNum();
    int myPoints = mySold * SOLD_POINTS + myDamka * DAMKA_POINTS;
    int oppPoints = oppSold * SOLD_POINTS + oppDamka * DAMKA_POINTS;
    if (myPoints == oppPoints) return 0;
    return (myPoints > oppPoints) ? (sourceEstimate/10) : (-sourceEstimate/10);
}

int CalculateStep::endGameOppDamkaAdjust(Statistic* statistic, int sourceEstimate)
{
    int myDamka = statistic->getMyDamkaNum();
    int oppDamka = statistic->getOppDamkaNum();
    if (myDamka > 2 && oppDamka == 1) return sourceEstimate * 11 / 10;
    return 0;
}

int CalculateStep::endGameMyDamkaAdjust(Statistic* statistic, int sourceEstimate)
{
    int myDamka = statistic->getMyDamkaNum();
    int oppDamka = statistic->getOppDamkaNum();
    if (oppDamka > 2 && myDamka == 1) return sourceEstimate * 11 / 10;
    return 0;
}
