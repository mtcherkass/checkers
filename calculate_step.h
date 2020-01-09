// calculate_step.h
#ifndef CALCULATE_STEP_H
#define CALCULATE_STEP_H

#include <fstream>

#include "cell_manager.h"
#include "state_check.h"
#include "step.h"
#include "statistic.h"


class CalculateStep
{
    CellManager *mCellMgr;
    ofstream mLogFile;

    const char* getStepLogString(const BaseStep& step);
    bool collectAvailableSteps(StateCheck* stateChk, bool userBottomSide, int level,
                               vector<BaseStep>& allStepsArr);
    bool generateRegularStep(const Position& pos, bool userBottomSide,
                             int direction, int stepVal, vector<BaseStep>& bestStepsArr);
    void calculateFishkaEstimate(const Position& pos, bool userBottomSide,
                                 vector<BaseStep>& bestStepsArr);
    void generateAllRegularSteps(const Position& pos, bool userBottomSide, int level,
                               vector<BaseStep> &bestStepsArr);
    void collectRegularSteps(bool userBottomSide, int level, vector<BaseStep>& stepsArr);
    int calculateEstimateForStep(StateCheck* stateChk, bool userBottomSide,
                                 int deep, int level, BaseStep &availableStep);
    int getEstimateByStatistics(Statistic* source, Statistic* dest,
                                bool userBottomSide, int deep, int level);
    void logOutBoard();
    int calc(int my, int opp, int estimateDelta);
    int hasIbetterStateAdjust(Statistic* statistic, int sourceEstimate);
    void updateEstimateForSoldInRow(Statistic* source, Statistic* dest,
                                    bool userBottomSide, int& estimate);
    int endGameOppDamkaAdjust(Statistic* statistic, int sourceEstimate);
    int endGameMyDamkaAdjust(Statistic* statistic, int sourceEstimate);
    void findSmallStepsArr(const vector<BaseStep>& oneDirStepsArr,
                           const Position& pos,
                           BaseStep oneDirSmallStepsArr[]);

public:
    CalculateStep(CellManager* cellMgr) : mCellMgr(cellMgr) {}
    int calculateBestStep(StateCheck* stateChk, bool userBottomSide, int deep,
                           int level, BaseStep& bestStep);
    bool hasAvailableSteps(StateCheck* stateChk, bool userBottomSide);
};

#endif // CALCULATE_STEP_H
