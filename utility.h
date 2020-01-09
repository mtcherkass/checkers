// utility.h
#ifndef UTILITY_H
#define UTILITY_H

#include <vector>
#include "constants.h"

using namespace std;

class UiEventsIntf;
class Position;

UiEventsIntf* uiEventsIntf(UiEventsIntf* intf = nullptr);

void showMsg(const char* msg);
void showErrorMsg(const char* msg);

//template <class T> void copyVector(vector<T>& from, vector<T>& to);
void copyPositionVector(vector<Position>& from, vector<Position>& to);
void showGameFinishDlg(GameFinish mode);

#endif // UTILITY_H
