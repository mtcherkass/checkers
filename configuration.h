// configuration.h
#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <QDialog>
#include <string>

using namespace std;

#define SOLD_BLUE "sold_blue"
#define SOLD_RED "sold_red"
#define DAMKA_BLUE "damka_blue"
#define DAMKA_RED "damka_red"

class ConfigurationData
{
    string mTopFishkaImageName;
    string mBottomFishkaImageName;
    string mTopDamkaImageName;
    string mBottomDamkaImageName;

    bool mIsUserBottomSide;
    bool mIsUserBlueColor;
    bool mIsUserStartsFirst;
    int mLevel;


public:
    ConfigurationData() :
        mIsUserBottomSide(true), mIsUserBlueColor(true),
        mIsUserStartsFirst(true), mLevel(2) { updateImages(); }

    void updateImages();
    const char* topFishkaImageName () { return mTopFishkaImageName.c_str(); }
    const char* bottomFishkaImageName() { return mBottomFishkaImageName.c_str(); }
    const char* topDamkaImageName () { return mTopDamkaImageName.c_str(); }
    const char* bottomDamkaImageName() { return mBottomDamkaImageName.c_str(); }

    bool isUserBottomSide() { return mIsUserBottomSide; }
    bool isUserStartFirst() { return mIsUserStartsFirst; }
    bool isUserBlueColor()  { return mIsUserBlueColor; }
    int getLevel() { return mLevel; }

    void setUserBottomSide(bool val) { mIsUserBottomSide = val; }
    void setUserStartFirst(bool val) { mIsUserStartsFirst = val; }
    void setUserBlueColor(bool val) { mIsUserBlueColor = val; }
    void setLevel(int val) { mLevel = val; }
};


namespace Ui {
class ConfigurationUI;
}

class ConfigurationUI : public QDialog
{
    Q_OBJECT

public:
    explicit ConfigurationUI(QWidget *parent = 0);
    ~ConfigurationUI();
    static void applyConfig(ConfigurationData* config);

private slots:
    void on_cancelPBtn_clicked();
    void on_okPBtn_clicked();

private:
    Ui::ConfigurationUI *ui;
    ConfigurationData* mConfig;
    static ConfigurationData mLocalConfigData;
};

#endif // CONFIGURATION_H
