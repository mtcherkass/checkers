// cell_manager.h
#ifndef CELL_DATA_H
#define CELL_DATA_H


#include <QGraphicsPixmapItem>
#include <vector>
#include "position.h"
#include "step.h"
#include "constants.h"

using namespace std;

class Cell{
    CellVal mVal;
    QGraphicsPixmapItem* mPixmap;

public:
    Cell() : mVal(EMPTY_CELL_VAL), mPixmap(nullptr) {}
    void set(const Cell& cell) { mVal = cell.mVal; mPixmap = cell.mPixmap; }
    void empty(bool full_empty = true);
    void setVal(CellVal val) { mVal = val; }
    CellVal getVal() { return mVal; }
    void setPixmapItem(QGraphicsPixmapItem* pixmap) { mPixmap = pixmap; }
    QGraphicsPixmapItem* pixmapItem() { return mPixmap; }
    void moveCellImage(const Position& fromPos, const Position& toPos);
    const Cell& operator=(const Cell& stp);
};


class CellManager {
    Cell mCellMatrix[BOARD_LEN][BOARD_LEN];
    Position mCurrentSelectedPos;
    unsigned int mRegularStepCount;

    void clean();
    void moveCellData(const Position& fromPos, const Position& toPos);

public:
    CellManager();
    CellManager(CellManager *mgr) : mRegularStepCount(0) { set(mgr); }
    void set(CellManager* mgr);
    void emptyDashboard();
    void initGame(bool userBottomSide, CellMgrInitObj* initObj = nullptr);
    void initCustomGame(CellMgrInitObj *initObj, bool userBottomSide);
    CellVal getCellVal(const Position& pos) { return mCellMatrix[pos.y()][pos.x()].getVal(); }
    void setCellVal(const Position& pos, CellVal val) { mCellMatrix[pos.y()][pos.x()].setVal(val); }
    QGraphicsPixmapItem* getCellPixmap(const Position& pos) { return mCellMatrix[pos.y()][pos.x()].pixmapItem(); }
    void setCellPixmap(int x, int y, QGraphicsPixmapItem* pixmap) { mCellMatrix[y][x].setPixmapItem(pixmap); }
    void moveSelectedCell(const Position& toPos);
    void moveCell(const Position& fromPos, const Position& toPos);
    void makeCalculatingStep(BaseStep &step);
    void revertStep(const BaseStep& step);
    void removeCell(const Position& pos, bool full_remove = true);
    const Position& getSelectedPos() { return mCurrentSelectedPos; }
    void updateSelPosition(const Position& pos) { mCurrentSelectedPos.set(pos); }
    void emptyCellSelection() { mCurrentSelectedPos.empty(); }
    bool hasSelectedFishka() { return (!mCurrentSelectedPos.isEmpty()); }
    bool isEmptyCell(const Position& pos);
    bool isFishkaPos(const Position& pos, bool userBottom);
    bool isSold(const Position& pos, bool userBottom);
    bool isDamka(const Position& pos, bool userBottom);
    bool isLegalPos(const Position& pos);
    bool convertToDamkaIfNeed(const Position& pos);
    void revertFromDamka(const Position& pos);
    bool isUserSideFishka(const Position& pos, bool userBottomSide);
    int soldNumInRow(int row, bool userBottomSide);
    bool reachedLastLine(const Position& pos);
    CellManager* clone();

    unsigned int getRegularStepCount() { return mRegularStepCount; }
    void resetRegularStepCount() { mRegularStepCount = 0; }
    void incRegularStepCount() { ++mRegularStepCount; }
};

bool operator==(const Position& pos1, const Position& pos2);

#endif // CELL_DATA_H
