/****************************************************************************
 *  @file     savesetting.h
 *  @brief    设置窗口单个图片保存设置界面类
 *  @author   ZHU Leyan
 *  @email
 *  @version
 *  @date     2021.01.18
 *  Copyright (c) 2020 SmartMore All rights reserved.
 ****************************************************************************/
#ifndef SAVESETTING_H
#define SAVESETTING_H

#include "configdatamanager/configmanager.h"
#include <QWidget>

namespace Ui
{
class SaveSetting;
}

namespace vtk::display
{
using ConfigManager = vtk::config::ConfigManager;
using SaveConfigItem = vtk::config::SaveConfigItem;
class SaveSetting : public QWidget
{
    Q_OBJECT

public:
    explicit SaveSetting(QWidget *parent = nullptr);
    ~SaveSetting();
    void setTitle(QString);
    void initImageSaveConfigData(const SaveConfigItem &);
    const SaveConfigItem &saveImageSaveConfigData();
    bool validateSaveSetting();
    void disableFocus(int);

private slots:
    void slotToggleSavePic(bool);
    void slotToggleSaveOK(bool);
    void slotToggleSaveNG(bool);
    void slotToggleDeleteCondition(bool);

protected:
    bool eventFilter(QObject *wcg, QEvent *event) override;

private:
    Ui::SaveSetting *ui;
    SaveConfigItem m_image_save_config_data;

    void initConnections();
    void selectPath(int);
    void disableSavePic();
    void enableSavePic(bool);
    void disableSaveOK();
    void enableSaveOK(bool);
    void disableSaveNG();
    void enableSaveNG(bool);
    void disableDelete();
    void enableDelete();
    bool validateSaveNumber();
    bool validateSaveDay();
};
}  // namespace vtk::display
#endif  // SAVESETTING_H
