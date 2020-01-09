#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QPixmap>
#include <QMainWindow>
#include <vector>
#include <QGraphicsView>
#include "cell_manager.h"
#include "state_check.h"
#include "configuration.h"
#include "calculate_step.h"
#include "history.h"

using namespace std;

namespace Ui {
class MainWindow;
}

enum CellAttribute
{
    NO_ATTR,
    SELECTED_ATTR,
    KILLED_ATTR
};

class UiEventsIntf
{
public:
    virtual void cellSelectedAction(const Position& pos) = 0;
};


class MainWindow : public QMainWindow, public UiEventsIntf, public PainterIntf
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void cellSelectedAction(const Position& pos);
    void restruct(CellManager*);

protected:
    void customEvent(QEvent *event); // This overrides QObject::customEvent()

private slots:
    void on_actionExit_triggered();

    void on_actionStart_New_Game_triggered();
    void convertToDamkaIfNeed(const Position& pos);

    void on_actionConfiguration_triggered();

    void on_actionAbout_triggered();
    void movingFishkaCB();

    void on_actionUndo_step_triggered();
    void on_actionRedo_step_triggered();

    void on_actionRules_triggered();

    void on_actionCustomized_Game_triggered();

private:
    void drawBoard();
    void drawInitFishkas();
    const char* getImageNameforFishka(const Position& pos, CellAttribute attr = NO_ATTR);

    void drawCell(const Position& pos, CellAttribute attr = NO_ATTR);
    void moveCellImage(const Position& fromPos, const Position& toPos);
    void removeCellImage(const Position& pos);
    void movingFishka(const BaseStep& step, bool isComputerStep);
    void performStep(const BaseStep& step);
    void computerMove();
    void endGame(GameFinish finishType);
    void removeFishkaArr(const vector<FishkaPosition>& arr);
    void removeAllCellImages();
    void addImageItem(const char* imgName, int clmN, int row);
    void highlightCellsBeforeRemove(const vector<FishkaPosition>& arr);
    void updateUndoRedoAccess();
    void keyReleaseEvent (QKeyEvent *key);
    void performMove(const Position& newSelectedCellPos);
    void startNewGame(CellMgrInitObj* initObj);
    void showThinkingText(bool show);
    void attachImageToScene(const char* image, QGraphicsPixmapItem **r_item, int x);

    Ui::MainWindow* ui;
    QGraphicsScene* mScene;
    CellManager mCellMgr;
    StateCheck mStateCheck;
    ConfigurationData mConfig;
    bool mInGame;
    bool mCustomGame;
    QGraphicsPixmapItem* mBoardItem;
    QGraphicsPixmapItem* mRightSideBar;
    QGraphicsPixmapItem* mLeftSideBar;
    CalculateStep mCalculator;
    QTimer *mTimer;
    BaseStep mCBStoredStep;
    bool mCBStoredIsComputerStep;
    History mHistory;
};


#endif // MAINWINDOW_H
