// about_dlg.h
#ifndef ABOUT_DLG_H
#define ABOUT_DLG_H

#include <QDialog>

namespace Ui {
class about_dlg;
}

class about_dlg : public QDialog
{
    Q_OBJECT

public:
    about_dlg(QWidget *parent = 0) : QDialog(parent), ui(new Ui::about_dlg)
    {
        ui->setupUi(this);
    }

    ~about_dlg() { delete ui; }

private slots:
    void on_okBP_clicked() { close(); }

private:
    Ui::about_dlg *ui;
};

#endif // ABOUT_DLG_H
