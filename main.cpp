#include "mainwindow.h"
#include <QApplication>
#include <qmessagebox.h>
#include <vector>

UiEventsIntf* uiEventsIntf(UiEventsIntf* intf)
{
    static UiEventsIntf* sIntf = nullptr;
    if (intf != nullptr)
    {
        sIntf = intf;
    }

    return sIntf;
}

void showMsg(const char* msg)
{
    QMessageBox msgBox;
    msgBox.setText(QString(msg));
    msgBox.exec();
}

void showErrorMsg(const char* msg)
{
    string str("Fatal error. ", msg);
    showMsg(str.c_str());
}

/*
void BaseStepToEstimateStepArr(vector<BaseStep>& eatingArr, vector<EstimateStep>& estimateArr)
{
    EstimateStep estimate_step;
    for (BaseStep& stp : eatingArr)
    {
        estimate_step.set(stp.fromPos(), stp.toPos(), stp.arrToEat(), 0);
        estimateArr.push_back(estimate_step); // PROBLEM IS HERE
    }
}
*/

template <class T> void copyVector(vector<T>& from, vector<T>& to)
{
    unsigned int size = from.size();
    for (unsigned int i = 0; i < size; i++)
    {
        to.push_back(from.at(i));
    }
}

void copyPositionVector(vector<Position>& from, vector<Position>& to)
{
    unsigned int size = from.size();
    for (unsigned int i = 0; i < size; i++)
    {
        to.push_back(from.at(i));
    }
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow wnd;
    uiEventsIntf(&wnd);
    wnd.show();

    return a.exec();
}
