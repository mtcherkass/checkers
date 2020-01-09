// customgamedlg.h
#ifndef CUSTOMGAMEDLG_H
#define CUSTOMGAMEDLG_H

#include <QDialog>
#include "constants.h"

namespace Ui {
class CustomGameDlg;
}

class CustomGameDlg : public QDialog
{
    Q_OBJECT

public:
    explicit CustomGameDlg(CellMgrInitObj& initObj, bool& execute, QWidget *parent = 0);
    ~CustomGameDlg();

private slots:
    void on_okPB_clicked();

    void on_cancelPB_clicked();

    void on_myKingSB_valueChanged(int);

    void on_compKingSB_valueChanged(int);

    void on_myPiecesSB_valueChanged(int);

    void on_compPiecesSB_valueChanged(int);

private:
    bool validData();

    Ui::CustomGameDlg *ui;
    CellMgrInitObj& mInitObj;
    bool& mExecute;

};

#endif // CUSTOMGAMEDLG_H
