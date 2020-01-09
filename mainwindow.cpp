#include <QPointF>
#include <iostream>
#include <stdlib.h>
#include <qmessagebox.h>

#include "mainwindow.h"
#include "utility.h"
#include "ui_mainwindow.h"
#include "aboutdlg.h"
#include "rulesdlg.h"
#include "customgamedlg.h"

const static int SIDEBAR_WIDTH = 25;

const QEvent::Type COMPUTER_STEP_EVENT = static_cast<QEvent::Type>(QEvent::User + 1);

/////////////////////////////////////////////////////////////

void BoardGraphicsView::mousePressEvent (QMouseEvent* event)
{
    //const  QPointF &pos = event->localPos();

   // int x = event->scenePos().x();
    //int y = event->scenePos().y();
    Position pos(event->pos().rx() - SIDEBAR_WIDTH, event->pos().ry());
    uiEventsIntf()->cellSelectedAction(pos);
}

/////////////////////////////////////////////////////////////
class ComputerStepEvent : public QEvent
{
    public:
        ComputerStepEvent() : QEvent(COMPUTER_STEP_EVENT) {}
};

/////////////////////////////////////////////////////////////
/// \brief MainWindow::MainWindow
/////////////////////////////////////////////////////////////
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    mStateCheck(&mCellMgr),
    mInGame(false),
    mCustomGame(false),
    mBoardItem(nullptr),
    mCalculator(&mCellMgr),
    mCBStoredIsComputerStep(false),
    mHistory(this)
{
    ui->setupUi(this);

    mScene = new QGraphicsScene(ui->boardGraphicsView);
    ui->boardGraphicsView->setScene(mScene);
    mScene->setSceneRect(0, 0, SCREEN_WIDTH + SIDEBAR_WIDTH * 2, SCREEN_HEIGHT);

    drawBoard();
    drawInitFishkas();
    showThinkingText(false);

    mTimer = new QTimer(this);
    connect(mTimer, SIGNAL(timeout()), this, SLOT(movingFishkaCB()));
    updateUndoRedoAccess();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete mBoardItem;
    delete mScene;
}

void MainWindow::attachImageToScene(const char* imageName, QGraphicsPixmapItem** r_item, int x)
{
    QGraphicsPixmapItem* item = nullptr;
    QImage image(imageName);
    item = new QGraphicsPixmapItem(QPixmap::fromImage(image));
    item->setPos(x, 0);
    mScene->addItem(item);
    if (r_item) *r_item = item;
}

void MainWindow::drawBoard()
{
    attachImageToScene("board", &mBoardItem, SIDEBAR_WIDTH);
    attachImageToScene("thinking___right", &mLeftSideBar, 0);
    attachImageToScene("thinking___right", &mRightSideBar, SCREEN_WIDTH + SIDEBAR_WIDTH);
}

void MainWindow::drawInitFishkas()
{
    Position pos;
    for (int rowN = 0; rowN < BOARD_LEN; rowN++)
    {
        for (int clmnN = 0; clmnN < BOARD_LEN; clmnN++)
        {
            pos.set(clmnN, rowN);
            const char* imgName = getImageNameforFishka(pos, NO_ATTR);
            if (imgName)
            {
                addImageItem(imgName, clmnN, rowN);
            }
        }
    }

    ui->boardGraphicsView->show();
}

void MainWindow::restruct(CellManager* mgr)
{
    removeAllCellImages();
    mCellMgr.set(mgr);
    drawInitFishkas();
}

void MainWindow::addImageItem(const char* imgName, int clmnN, int rowN)
{
    QPixmap pixmap(imgName);
    QGraphicsPixmapItem* pixmapItem = new QGraphicsPixmapItem(pixmap);
    mCellMgr.setCellPixmap(clmnN, rowN, pixmapItem);
    mScene->addItem(pixmapItem);
    pixmapItem->setPos(clmnN * CELL_WIDTH + SIDEBAR_WIDTH, rowN * CELL_HEIGHT);
}

const char* MainWindow::getImageNameforFishka(const Position& pos, CellAttribute attr)
{
    static string imgName;

    CellVal val = mCellMgr.getCellVal(pos);
    string postfix;
    switch(attr)
    {
    case NO_ATTR: postfix = ".png"; break;
    case SELECTED_ATTR: postfix = "_selected.png"; break;
    case KILLED_ATTR: postfix = "_killed.png"; break;
    default: string msg("getImageNameforFishka: wrong attribute ", attr);
        showErrorMsg(msg.c_str());
        break;
    }

    switch (val)
    {
    case TOP_CELL_SOLD_VAL:
        imgName = mConfig.topFishkaImageName() + postfix;
        break;
    case BOTTOM_CELL_SOLD_VAL:
        imgName = mConfig.bottomFishkaImageName() + postfix;
        break;
    case TOP_CELL_DAMKA_VAL:
        imgName = mConfig.topDamkaImageName() + postfix;
        break;
    case BOTTOM_CELL_DAMKA_VAL:
        imgName = mConfig.bottomDamkaImageName() + postfix;
        break;

    default:
        return nullptr;
    }

    return imgName.c_str();
}

void MainWindow::drawCell(const Position& pos, CellAttribute attr)
{
    const char* imgName = getImageNameforFishka(pos, attr);
    if (imgName)
    {
        QPixmap pixmap(imgName);
        QGraphicsPixmapItem* pixmapItem = mCellMgr.getCellPixmap(pos);
        if (pixmapItem != nullptr)
        {
            pixmapItem->setPixmap(pixmap);
        }
    }
}

void MainWindow::moveCellImage(const Position& fromPos, const Position& toPos)
{
    QGraphicsPixmapItem* pixmapItem = mCellMgr.getCellPixmap(fromPos);
    if (pixmapItem != nullptr)
    {
        pixmapItem->setPos(toPos.x() * CELL_WIDTH + SIDEBAR_WIDTH, toPos.y() * CELL_HEIGHT);
    }
    else
    {
        showErrorMsg("Error. moveCellImage: Access to empty pixmap");
    }
}

void MainWindow::removeCellImage(const Position& pos)
{
    QGraphicsPixmapItem* pixmapItem = mCellMgr.getCellPixmap(pos);
    if (pixmapItem)
    {
        delete pixmapItem;
        mCellMgr.setCellPixmap(pos.x(), pos.y(), nullptr);
    }
}

void MainWindow::highlightCellsBeforeRemove(const vector<FishkaPosition>& arr)
{
    for (const FishkaPosition& pos : arr)
    {
        drawCell(pos, KILLED_ATTR);
    }
}

void MainWindow::cellSelectedAction(const Position& selectedPixelsPos)
{
    Position newSelectedCellPos(selectedPixelsPos);
    newSelectedCellPos.convertPixelToCell();

    if (!mCellMgr.hasSelectedFishka()) // no selected fishka, so need to select it
    {
        if (mStateCheck.isCellLegalForSelect(newSelectedCellPos, mConfig.isUserBottomSide()))
        {
            drawCell(newSelectedCellPos, SELECTED_ATTR);
            mCellMgr.updateSelPosition(newSelectedCellPos);
        }
    }
    else // has selected fishka
    {
        bool whiteCellSelected = mCellMgr.getCellVal(newSelectedCellPos) == WHITE_CELL_VAL;
        bool newSelection = mCellMgr.isUserSideFishka(newSelectedCellPos, mConfig.isUserBottomSide());
        bool oppCellSelected = mCellMgr.isUserSideFishka(newSelectedCellPos, !mConfig.isUserBottomSide());

        // Unselecting action
        if (whiteCellSelected || newSelection || oppCellSelected)
        {
            const Position& currentSelectedPos = mCellMgr.getSelectedPos();
            drawCell(currentSelectedPos, NO_ATTR);
            mCellMgr.emptyCellSelection();
            if (newSelection) // perform new selection
            {
                cellSelectedAction(selectedPixelsPos);
            }
        }
        else  // Moving fishka
        {
            performMove(newSelectedCellPos);
        }
    }
}

void MainWindow::performMove(const Position& newSelectedCellPos)
{
    const Position& currentSelectedPos = mCellMgr.getSelectedPos();
    vector<FishkaPosition> fishkaArrToRemove;

    StepErrorType errType = mStateCheck.stepVerify(
                currentSelectedPos, newSelectedCellPos,
                mConfig.isUserBottomSide(), fishkaArrToRemove);

    switch(errType)
    {
    case OK_STEP:
    {
        BaseStep step(currentSelectedPos, newSelectedCellPos);
        step.setEatingArray(fishkaArrToRemove);

        mInGame = true;
        performStep(step);
        break;
    }
    case ILLEGAL_STEP:
        showMsg("This move is not permitted");
        break;
    case HAVE_BETTER_BEAT_STEP:
        showMsg("This beating move is not the best one.");
        break;
    case MUST_BEAT_ERROR_STEP:
        showMsg("Beating move has to be done");
        break;
    }
}

void MainWindow::computerMove()
{
    Position from;
    Position to;
    vector<FishkaPosition> fishkaArrForRemove;
    BaseStep bestStep;
    bool isComputerBottom = mConfig.isUserBottomSide() ? false : true;

    bool bestStepFound = mCalculator.calculateBestStep(&mStateCheck, isComputerBottom,
                                                       0, mConfig.getLevel(), bestStep) != LOSE_ESTIMATE;

    if (bestStepFound)
    {
        drawCell(bestStep.fromPos(), SELECTED_ATTR);
        mCBStoredStep = bestStep;
        mCBStoredIsComputerStep = true;

        if (!bestStep.arrToEat().empty()) // eating step
        {
            highlightCellsBeforeRemove(bestStep.arrToEat());

            mTimer->start(700);
            mCellMgr.resetRegularStepCount();
        }
        else // regular step
        {
            mTimer->start(700);
            mCellMgr.incRegularStepCount();
        }

    }
    else
    {
        endGame(GAME_WIN);
    }
}

void MainWindow::movingFishkaCB()
{
    mTimer->stop();
    movingFishka(mCBStoredStep, mCBStoredIsComputerStep);
}

// first call is user call, isComputerStep=false
// second call is computer call, isComputerStep=true
void MainWindow::movingFishka(const BaseStep& bestStep, bool isComputerStep)
{
    drawCell(bestStep.fromPos(), NO_ATTR); // reset from cell, which has selected image

    removeFishkaArr(bestStep.arrToEat());

    if (!isComputerStep)
    {
        computerMove();
        //QApplication::postEvent(this, new ComputerStepEvent());
    }
    else // computer call
    {
        showThinkingText(false);
        moveCellImage(bestStep.fromPos(), bestStep.toPos());
        mCellMgr.moveCell(bestStep.fromPos(), bestStep.toPos());

        convertToDamkaIfNeed(bestStep.toPos());

        // record history at finish of user/computer steps processing
        mHistory.addNewMap(&mCellMgr);
        updateUndoRedoAccess();

        if (!mCalculator.hasAvailableSteps(&mStateCheck, mConfig.isUserBottomSide()))
        {
            endGame(GAME_LOST);
        }
        else if (mStateCheck.isTie() && mCellMgr.getRegularStepCount() > 2)
        {
            endGame(GAME_NO_WINNER);
        }
        else if (mCellMgr.getRegularStepCount() > 31)
        {
            endGame(GAME_NO_WINNER_TOO_MANY_STEPS);
        }
    }
}

void MainWindow::endGame(GameFinish mode)
{
    showGameFinishDlg(mode);

    if (mCustomGame)
    {
        QMessageBox msgBox;
        msgBox.setText("Do you want to play again a customized game");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::Yes);
        int ret = msgBox.exec();
        if (ret == QMessageBox::Yes)
        {
            on_actionCustomized_Game_triggered();
        }
        else
        {
            on_actionStart_New_Game_triggered();
        }
    }
    else
    {
        on_actionStart_New_Game_triggered();
    }
}

void MainWindow::performStep(const BaseStep& step)
{
     // unselect selected fishka (UI draw)
    drawCell(step.fromPos(), NO_ATTR);

    // move the fishka
    moveCellImage(step.fromPos(), step.toPos()); // UI update
    mCellMgr.moveSelectedCell(step.toPos()); // DB update

    // check if reached last line $ should convert it to Damka
    convertToDamkaIfNeed(step.toPos());

    // unselect selected fishka (DB update)
    mCellMgr.emptyCellSelection();

    showThinkingText(true);
    int delayTime = 1;
    if (!step.arrToEat().empty()) // eating step
    {
        highlightCellsBeforeRemove(step.arrToEat());
        mCellMgr.resetRegularStepCount();
        delayTime = 700;
    }
    else // regular step
    {
        //movingFishka(step, false);
        mCellMgr.incRegularStepCount();
    }
    mCBStoredStep = step;
    mCBStoredIsComputerStep = false;
    mTimer->start(delayTime);
}

void MainWindow::convertToDamkaIfNeed(const Position& pos)
{
    if (mCellMgr.convertToDamkaIfNeed(pos))
    {
        drawCell(pos);
    }
}

void MainWindow::removeFishkaArr(const vector<FishkaPosition>& arr)
{
    unsigned int i = 0;
    const unsigned int size = arr.size();

    for (i = 0; i < size; i++)
    {
        removeCellImage(arr.at(i));
        mCellMgr.removeCell(arr.at(i));
    }
}

void MainWindow::removeAllCellImages()
{
    Position pos;
    for (int rowN = 0; rowN < BOARD_LEN; rowN++)
    {
        for (int clmnN = 0; clmnN < BOARD_LEN; clmnN++)
        {
            pos.set(clmnN, rowN);
            removeCellImage(pos);
        }
    }
}

void MainWindow::on_actionExit_triggered()
{
    exit(0);
}

void MainWindow::on_actionStart_New_Game_triggered()
{
    startNewGame(nullptr);
}

void MainWindow::on_actionCustomized_Game_triggered()
{
    CellMgrInitObj initObj(1, 2, 1, 2);
    bool execute = true;
    CustomGameDlg customDlg(initObj, execute);
    customDlg.setModal(true);
    customDlg.exec();
    if (execute)
    {
        startNewGame(&initObj);
        mCustomGame = true;
    }
}

void MainWindow::startNewGame(CellMgrInitObj* initObj)
{
    removeAllCellImages();
    ConfigurationUI::applyConfig(&mConfig);
    mCellMgr.initGame(mConfig.isUserBottomSide(), initObj);
    mCellMgr.emptyCellSelection();
    drawInitFishkas();
    mInGame = false; // until first step done it is not "inGame" state
    mCustomGame = false;
    mCellMgr.resetRegularStepCount();
    mHistory.clean();
    updateUndoRedoAccess();
}

void MainWindow::on_actionConfiguration_triggered()
{
    ConfigurationUI configDlg;
    configDlg.setModal(true);
    configDlg.exec();
}

void MainWindow::on_actionAbout_triggered()
{
    aboutDlg aboutDlg;
    aboutDlg.setModal(true);
    aboutDlg.exec();
}

void MainWindow::on_actionRules_triggered()
{
    RulesDlg rulesDlg;
    rulesDlg.setModal(true);
    rulesDlg.exec();
}

void MainWindow::on_actionUndo_step_triggered()
{
    if (mHistory.canUndo()) mHistory.undo();
    updateUndoRedoAccess();
}

void MainWindow::on_actionRedo_step_triggered()
{
    if (mHistory.canRedo()) mHistory.redo();
    updateUndoRedoAccess();
}

void MainWindow::updateUndoRedoAccess()
{
    ui->actionUndo_step->setEnabled(mHistory.canUndo());
    ui->actionRedo_step->setEnabled(mHistory.canRedo());
}

void MainWindow::keyReleaseEvent( QKeyEvent *key )
{
    if (key->modifiers() == Qt::CTRL)
    {
        switch (key->key())
        {
        case Qt::Key_Z:
            on_actionUndo_step_triggered();
            break;
        case Qt::Key_Y:
            on_actionRedo_step_triggered();
            break;
        }
    }
}

void MainWindow::showThinkingText(bool show)
{
    if (show)
    {
        mLeftSideBar->show();
        mRightSideBar->show();
    }
    else
    {
        mLeftSideBar->hide();
        mRightSideBar->hide();
    }

  //  ui->thinkingLeftLB->setVisible(show);
  //  ui->thinkingRightLB->setVisible(show);
}

void MainWindow::customEvent(QEvent * event)
{
    if(event->type() == COMPUTER_STEP_EVENT)
    {
        computerMove();
    }
}
