#include "customgamedlg.h"
#include "ui_customgamedlg.h"

CustomGameDlg::CustomGameDlg(CellMgrInitObj& initObj, bool &execute, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CustomGameDlg),
    mInitObj(initObj),
    mExecute(execute)
{
    ui->setupUi(this);
    ui->myKingSB->setValue(initObj.userKingsNum);
    ui->myPiecesSB->setValue(initObj.userPiecesNum);
    ui->compKingSB->setValue(initObj.compKingsNum);
    ui->compPiecesSB->setValue(initObj.compPiecesNum);
}

CustomGameDlg::~CustomGameDlg()
{
    delete ui;
}

void CustomGameDlg::on_okPB_clicked()
{
    mInitObj.userKingsNum = ui->myKingSB->value();
    mInitObj.userPiecesNum = ui->myPiecesSB->value();
    mInitObj.compKingsNum = ui->compKingSB->value();
    mInitObj.compPiecesNum = ui->compPiecesSB->value();
    mExecute = true;
    close();
}

bool CustomGameDlg::validData()
{
    int mySum = ui->myKingSB->value() + ui->myPiecesSB->value();
    int compSum = ui->compKingSB->value() + ui->compPiecesSB->value();
    if (mySum > 2 && compSum > 0) return true;
    if (compSum > 2 && mySum > 0) return true;
    return false;
}

void CustomGameDlg::on_cancelPB_clicked()
{
    mExecute = false;
    close();
}

void CustomGameDlg::on_myKingSB_valueChanged(int)
{
    ui->okPB->setEnabled(validData());
}

void CustomGameDlg::on_compKingSB_valueChanged(int)
{
   ui->okPB->setEnabled(validData());
}

void CustomGameDlg::on_myPiecesSB_valueChanged(int)
{
    ui->okPB->setEnabled(validData());
}

void CustomGameDlg::on_compPiecesSB_valueChanged(int)
{
    ui->okPB->setEnabled(validData());
}
