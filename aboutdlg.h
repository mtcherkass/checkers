#ifndef ABOUTDLG_H
#define ABOUTDLG_H

#include <QDialog>
#include "ui_aboutdlg.h"

namespace Ui {
class aboutDlg;
}

class aboutDlg : public QDialog
{
    Q_OBJECT

public:
    explicit aboutDlg(QWidget *parent = 0) :
        QDialog(parent),
        ui(new Ui::aboutDlg)
    {
        ui->setupUi(this);
    }

    ~aboutDlg()
    {
        delete ui;
    }

private slots:
    void on_okBP_clicked()
    {
        close();
    }

private:
    Ui::aboutDlg *ui;
};

#endif // ABOUTDLG_H
