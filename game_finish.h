#ifndef GAME_FINISH_H
#define GAME_FINISH_H

#include <QDialog>
#include "constants.h"

namespace Ui {
class GameFinishDlg;
}

class GameFinishDlg : public QDialog
{
    Q_OBJECT

public:
    explicit GameFinishDlg(GameFinish mode, QWidget *parent = 0);
    ~GameFinishDlg();

private slots:
    void on_okPB_clicked();

private:
    Ui::GameFinishDlg *ui;
};



#endif // GAME_FINISH_H
