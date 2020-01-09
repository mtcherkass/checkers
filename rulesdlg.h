#ifndef RULESDLG_H
#define RULESDLG_H

#include <QDialog>
#include "ui_rulesdlg.h"

namespace Ui {
class RulesDlg;
}

class RulesDlg : public QDialog
{
    Q_OBJECT

public:
    explicit RulesDlg(QWidget *parent = 0) :
        QDialog(parent),
        ui(new Ui::RulesDlg)
    {
        ui->setupUi(this);
    }

    ~RulesDlg()
    {
        delete ui;
    }

private slots:
    void on_pushButton_clicked()
    {
        close();
    }

private:
    Ui::RulesDlg *ui;
};

#endif // RULESDLG_H
