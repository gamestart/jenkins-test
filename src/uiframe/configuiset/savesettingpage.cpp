/****************************************************************************
 *  @file     savesettingpage.cpp
 *  @brief    设置窗口保存设置界面类
 *  @author
 *  @email
 *  @version
 *  @date     2020.12.21
 *  Copyright (c) 2020 SmartMore All rights reserved.
 ****************************************************************************/
#include <QFileDialog>
#include <QListView>

#include "savesettingpage.h"
#include "ui_savesettingpage.h"

namespace vtk::display
{
using ConfigManager = vtk::config::ConfigManager;
using SaveConfigItem = vtk::config::SaveConfigItem;

SaveSettingPage::SaveSettingPage(QWidget *parent) : QWidget(parent), ui(new Ui::SaveSettingPage)
{
    ui->setupUi(this);
    ui->save_opic_widget->setTitle(tr("Original Image"));
    ui->save_rpic_widget->setTitle(tr("AI Image"));
}

SaveSettingPage::~SaveSettingPage()
{
    delete ui;
}

void SaveSettingPage::initImageSaveConfigData()
{
    const auto &image_save_config_data = ConfigManager::getInstance().getImageSaveConfigData();
    ui->save_opic_widget->initImageSaveConfigData(image_save_config_data.original);
    ui->save_rpic_widget->initImageSaveConfigData(image_save_config_data.rendered);
}

void SaveSettingPage::cancelImageSaveConfig()
{
    initImageSaveConfigData();
}

void SaveSettingPage::saveImageSaveConfigData()
{
    SaveConfigData data;
    data.original = ui->save_opic_widget->saveImageSaveConfigData();
    data.rendered = ui->save_rpic_widget->saveImageSaveConfigData();
    ConfigManager::getInstance().setImageSaveConfigData(data);
}

bool SaveSettingPage::validateSaveSettings()
{
    int flag = 0;
    if (!ui->save_opic_widget->validateSaveSetting())
        flag++;
    if (!ui->save_rpic_widget->validateSaveSetting())
        flag++;
    if (flag)
        return false;
    else
        return true;
}

void SaveSettingPage::disableFocus()
{
    ui->save_opic_widget->disableFocus(1);
    ui->save_rpic_widget->disableFocus(1);
    ui->save_opic_widget->disableFocus(2);
    ui->save_rpic_widget->disableFocus(2);
}
}  // namespace vtk::display
