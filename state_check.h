// state_check.h
#ifndef STATE_CHECK_H
#define STATE_CHECK_H

#include "cell_manager.h"
#include "step.h"

#define  NO_DIRECTION            -1
#define  RIGHT_FORWARD_DIRECTION  0
#define  LEFT_FORWARD_DIRECTION   1
#define  RIGHT_BACK_DIRECTION     2
#define  LEFT_BACK_DIRECTION      3
#define  DIRECTION_NUM            4

enum StepErrorType
{
    OK_STEP,
    ILLEGAL_STEP,
    HAVE_BETTER_BEAT_STEP,
    MUST_BEAT_ERROR_STEP
};

enum EatingStatus
{
    EATING_NOT_FOUND,
    DOES_NOT_EAT_BUT_CAN,
    EAT_NOT_MAXIMUM,
    EATING_OK
};


//======================================================
class StateCheck
{
private:
    CellManager* mCellMgr;

    bool isRegularStep(const Position& currentSelectedPos,
                       const Position& newSelectedCellPos, bool userBottomSide);
    EatingStatus verifyEatingStep(const Position& currentSelectedPos,
                       const Position& newSelectedCellPos, bool userBottomSide,
                       vector<FishkaPosition>& eatingFishkaArr);
    bool userBottomSide(const Position& pos);
    bool isLegalPos(const Position& pos);
    void findFishkaEatingSteps(const BaseStep& step, int max_step, int prev_direction,
                               bool userBottomSide, vector<BaseStep> eatingPosArr[],
                               int stepNum = 0, bool only_forward_dir = false);
    int getOppositeDirection(int direcion);
    bool findOppositePosInChain(const Position& pos, int interval,
                                int direction, bool userBottomSide, FishkaPosition &oppositerPos);
    bool findEatingSteps(const BaseStep& step, int stepVal,
                         int stepNum, int max_step, int direction, bool userBottomSide,
                         vector<BaseStep> eatingStepsArr[]);
    bool isPositionExist(const vector<FishkaPosition>& arr, const FishkaPosition& pos);

public:
    StateCheck(CellManager* mgr) : mCellMgr(mgr) {}
    bool isCellLegalForSelect(const Position& pos, bool userBottomSide);
    bool findBestEatingArray(bool userBottomSide, vector<BaseStep>& eatingFishkaArr);
    StepErrorType stepVerify(const Position& currentSelectedPos,
                     const Position& newSelectedCellPos, bool userBottomSide,
                     vector<FishkaPosition>& eatingFishkaArr);
    CellManager* getManager() { return mCellMgr; }
    void findAllEatingSteps(bool userBottomSide, vector<BaseStep> eatingStepsArr[]);
    bool isTie();
};

//======================================================
class CheckIntf {
public:
    virtual ~CheckIntf() {}
    virtual bool isRegularStep(const Position& currentSelectedPos,
                       const Position& newSelectedCellPos, bool userBottomSide) = 0;
};

//======================================================
class SoldCheck : public CheckIntf
{
public:
    bool isRegularStep(const Position& currentSelectedPos,
                       const Position& newSelectedCellPos, bool userBottomSide);
};

//======================================================
class DamkaCheck : public CheckIntf
{
    StateCheck *mCheckObj;
    void collectStepsForDirection(int xDir, int yDir, const Position& pos,
                                  vector<Position>& possibleStepPosArr);
public:
    DamkaCheck(StateCheck* chkObj) : mCheckObj(chkObj) {}
    bool isRegularStep(const Position& currentSelectedPos,
                       const Position& newSelectedCellPos, bool);
};

#endif // STATE_CHECK_H
