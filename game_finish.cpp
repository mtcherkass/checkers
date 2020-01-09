#include "game_finish.h"
#include "ui_game_finish.h"

GameFinishDlg::GameFinishDlg(GameFinish mode, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GameFinishDlg)
{
    ui->setupUi(this);

    QString title;
    QString img;

    switch (mode)
    {
    case GAME_WIN:
        title = "Victory !!!";
        img = "victory.png";
        break;
    case GAME_LOST:
        title = "Game lost";
        img = "loser.png";
        break;
    case GAME_NO_WINNER:
        title = "No winner. Drawn Game.";
        img = "tie.png";
        break;
    case GAME_NO_WINNER_TOO_MANY_STEPS:
        title = "Drawn Game. 15 moves without action";
        img = "tie.png";
        break;
    }

    ui->label->setText(title);
    QImage qimage(img);
    ui->image->setPixmap(QPixmap::fromImage(qimage));
}

GameFinishDlg::~GameFinishDlg()
{
    delete ui;
}

void GameFinishDlg::on_okPB_clicked()
{
    close();
}

void showGameFinishDlg(GameFinish mode)
{
    GameFinishDlg finishDlg(mode);
    finishDlg.setModal(true);
    finishDlg.exec();
}
