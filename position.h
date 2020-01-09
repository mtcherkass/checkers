#ifndef POSITION_H
#define POSITION_H

#include "constants.h"

class Position
{
    int mx;
    int my;

public:
    Position() { empty(); }
    Position(const Position& pos) : mx(pos.mx), my(pos.my) {}
    Position(int x, int y) : mx(x), my(y) {}
    void empty() { mx = -1; my = -1; }
    bool isEmpty() { return (mx == -1 && my == -1); }
    void set(int xVal, int yVal) { mx = xVal; my = yVal; }
    void set(const Position& pos) { mx = pos.mx; my = pos.my; }
    void changeBy(int x, int y) { mx += x; my += y; }
    void convertPixelToCell() { mx /= CELL_WIDTH; my /= CELL_HEIGHT; }
    int x() const { return mx; }
    int y() const { return my; }
};

class FishkaPosition : public Position
{
    CellVal mFishkaVal;

public:
    FishkaPosition() : Position(), mFishkaVal(EMPTY_CELL_VAL) {}
    FishkaPosition(const Position& pos) { set(pos); }
    void setFishkaVal(CellVal val) { mFishkaVal = val; }
    CellVal getFishkaVal() const { return mFishkaVal; }
};


#endif // POSITION_H
