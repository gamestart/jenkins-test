/****************************************************************************
 *  @file     viewsettingpage.h
 *  @brief    设置窗口显示设置界面类
 *  @author
 *  @email
 *  @version
 *  @date     2020.12.21
 *  Copyright (c) 2020 SmartMore All rights reserved.
 ****************************************************************************/
#ifndef VIEWSETTINGPAGE_H
#define VIEWSETTINGPAGE_H

#include "configdatamanager/configmanager.h"
#include "viewsetting.h"

#include <QGridLayout>
#include <QLabel>
#include <QListWidget>
#include <QWidget>

namespace Ui
{
class ViewSettingPage;
}

namespace vtk::display
{
class CameraSetting;
class ViewSettingPage : public QWidget
{
    Q_OBJECT

public:
    explicit ViewSettingPage(QWidget *parent = nullptr);
    ~ViewSettingPage();
    bool validateViewSettings();
    void initViewConfigData();
    void saveViewConfigData();
    void cancelViewConfig();
    void disableFocus();
    void updateViewSources();
    void updateViewModels();

private:
    Ui::ViewSettingPage *ui;
    void addView(vtk::config::ViewConfigItem);
};
}  // namespace vtk::display
#endif  // VIEWSETTINGPAGE_H
