/****************************************************************************
 *  @file     systemsettingpage.h
 *  @brief    设置窗口系统设置界面类
 *  @author
 *  @email
 *  @version
 *  @date     2020.12.21
 *  Copyright (c) 2020 SmartMore All rights reserved.
 ****************************************************************************/
#ifndef SYSTEMSETTINGPAGE_H
#define SYSTEMSETTINGPAGE_H

#include <QWidget>

namespace Ui
{
class SystemSettingPage;
}

namespace vtk::display
{

class SystemSettingPage : public QWidget
{
    Q_OBJECT

public:
    explicit SystemSettingPage(QWidget *parent = nullptr);
    ~SystemSettingPage();
    void initSystemConfigData();
    void saveSystemConfigData();
    bool validateSystemName();
    void cancelSystemConfig();
    void disableFocus();

private:
    Ui::SystemSettingPage *ui;
    bool eventFilter(QObject *wcg, QEvent *event) override;
};
}  // namespace vtk::display
#endif  // SYSTEMSETTINGPAGE_H
