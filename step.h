#ifndef STEP_H
#define STEP_H

#include "position.h"
#include <vector>

using namespace std;


class BaseStep
{
    Position mFrom;
    Position mTo;
    vector<FishkaPosition> mFishkaArrToEat;
    bool mConvertToDamka;

public:
    BaseStep() : mConvertToDamka(false) { mFrom.set(0, 0); mTo.set(0, 0); }
    BaseStep(const BaseStep& step)
    {
        setPos(step.mFrom, step.mTo);
        setEatingArray(step.mFishkaArrToEat);
        mConvertToDamka = step.mConvertToDamka;
    }
    BaseStep(const Position& from, const Position& to) : mFrom(from), mTo(to), mConvertToDamka(false) {}

    void setPos(const Position& from, const Position& to) { mFrom.set(from); mTo.set(to); }
    void setEatingArray(const vector<FishkaPosition>& arr)  { mFishkaArrToEat = arr; }
    void setConvertDoDamka() { mConvertToDamka = true; }

    const Position& fromPos() const { return mFrom; }
    const Position& toPos() const { return mTo; }
    const vector<FishkaPosition>& arrToEat() const { return mFishkaArrToEat; }
    vector<FishkaPosition>& arrToEatRef() { return mFishkaArrToEat; }
    bool shouldConvertToDamka() const { return mConvertToDamka; }
    const BaseStep& operator=(const BaseStep& stp)
    {
        setPos(stp.fromPos(), stp.toPos());
        setEatingArray(stp.arrToEat());
        return *this;
    }
};

#endif // STEP_H
