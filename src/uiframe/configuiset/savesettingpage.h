/****************************************************************************
 *  @file     savesettingpage.h
 *  @brief    设置窗口保存设置界面类
 *  @author
 *  @email
 *  @version
 *  @date     2020.12.21
 *  Copyright (c) 2020 SmartMore All rights reserved.
 ****************************************************************************/
#ifndef SAVESETTINGPAGE_H
#define SAVESETTINGPAGE_H

#include <QWidget>

#include "configdatamanager/configmanager.h"

namespace Ui
{
class SaveSettingPage;
}

namespace vtk::display
{

using SaveConfigData = vtk::config::SaveConfigData;
class SaveSettingPage : public QWidget
{
    Q_OBJECT

public:
    explicit SaveSettingPage(QWidget *parent = nullptr);
    ~SaveSettingPage();
    void initImageSaveConfigData();
    void saveImageSaveConfigData();
    bool validateSaveSettings();
    void cancelImageSaveConfig();
    void disableFocus();

private:
    Ui::SaveSettingPage *ui;
    SaveConfigData m_image_save_config_data;
};
}  // namespace vtk::display
#endif  // SAVESETTINGPAGE_H
