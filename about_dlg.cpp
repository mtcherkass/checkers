#include "about_dlg.h"
#include "ui_about_dlg.h"



void about_dlg::on_okBP_clicked()
{
    close();
}

void about_dlg::on_about_dlg_destroyed()
{
    close();
}
