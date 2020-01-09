#include "configuration.h"
#include "ui_configuration.h"

ConfigurationData ConfigurationUI::mLocalConfigData; // static variable definition
/////////////////////////////////////////////////////////////

void ConfigurationData::updateImages()
{
    if ((mIsUserBlueColor && mIsUserBottomSide) ||
            (!mIsUserBlueColor && !mIsUserBottomSide))
    {
        mBottomFishkaImageName = SOLD_BLUE;
        mTopFishkaImageName = SOLD_RED;
        mBottomDamkaImageName = DAMKA_BLUE;
        mTopDamkaImageName = DAMKA_RED;
    }
    else
    {
        mBottomFishkaImageName = SOLD_RED;
        mTopFishkaImageName = SOLD_BLUE;
        mBottomDamkaImageName = DAMKA_RED;
        mTopDamkaImageName = DAMKA_BLUE;
    }
}

/////////////////////////////////////////////////////////////

ConfigurationUI::ConfigurationUI(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConfigurationUI)
{
    ui->setupUi(this);
    ui->startFirstChkBox->setChecked(mLocalConfigData.isUserStartFirst());

    ui->bottomSideRadio->setChecked(mLocalConfigData.isUserBottomSide());
    ui->topSideRadio->setChecked(!mLocalConfigData.isUserBottomSide());

    ui->blueColorRadio->setChecked(mLocalConfigData.isUserBlueColor());
    ui->redColorRadio->setChecked(!mLocalConfigData.isUserBlueColor());

    ui->beginnerRadio->setChecked(mLocalConfigData.getLevel() == 0);
    ui->mediumRadio->setChecked(mLocalConfigData.getLevel() == 1);
    ui->masterRadio->setChecked(mLocalConfigData.getLevel() == 2);
}

ConfigurationUI::~ConfigurationUI()
{
    delete ui;
}

void ConfigurationUI::on_cancelPBtn_clicked()
{
    close();
}

void ConfigurationUI::on_okPBtn_clicked()
{
    mLocalConfigData.setUserBottomSide(ui->bottomSideRadio->isChecked());
    mLocalConfigData.setUserStartFirst(ui->startFirstChkBox->isChecked());
    mLocalConfigData.setUserBlueColor(ui->blueColorRadio->isChecked());

    bool medium = ui->mediumRadio->isChecked();
    bool master = ui->masterRadio->isChecked();
    int level = 0; // beginner by default
    if (medium) level = 1;
    if (master) level = 2;
    mLocalConfigData.setLevel(level);

    close();
}

void ConfigurationUI::applyConfig(ConfigurationData* config)
{
    config->setUserBottomSide(mLocalConfigData.isUserBottomSide());
    config->setUserStartFirst(mLocalConfigData.isUserStartFirst());
    config->setUserBlueColor(mLocalConfigData.isUserBlueColor());
    config->setLevel(mLocalConfigData.getLevel());
    config->updateImages();
}
