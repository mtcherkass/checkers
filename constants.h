// constants.h
#ifndef CONSTANTS_H
#define CONSTANTS_H

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 640;

const int BOARD_LEN = 8;

const int CELL_WIDTH = SCREEN_WIDTH / BOARD_LEN;
const int CELL_HEIGHT = SCREEN_HEIGHT / BOARD_LEN;

#define FISHKA_COUNT 12

#define SMALLEST_ESTIMATE -9999
#define LOSE_ESTIMATE -5000
#define WIN_ESTIMATE   5000
#define BIGGEST_ESTIMATE 9999
#define VERY_LOW_ESTIMATE -1000
#define VERY_HIGH_ESTIMATE 1000
#define ENDSHPIL_WIN_ESTIMATE 600

enum GameFinish
{
    GAME_WIN,
    GAME_LOST,
    GAME_NO_WINNER,
    GAME_NO_WINNER_TOO_MANY_STEPS
};

enum CellVal
{
    WHITE_CELL_VAL,
    EMPTY_CELL_VAL,
    TOP_CELL_SOLD_VAL,
    TOP_CELL_DAMKA_VAL,
    BOTTOM_CELL_SOLD_VAL,
    BOTTOM_CELL_DAMKA_VAL
};

struct CellMgrInitObj
{
    int userKingsNum;
    int userPiecesNum;
    int compKingsNum;
    int compPiecesNum;

    CellMgrInitObj(int ukn, int upn, int ckn, int cpn) :
        userKingsNum(ukn), userPiecesNum(upn), compKingsNum(ckn), compPiecesNum(cpn) {}
};


#endif // CONSTANTS_H
