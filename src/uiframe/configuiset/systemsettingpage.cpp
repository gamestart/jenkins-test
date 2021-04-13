/****************************************************************************
 *  @file     systemsettingpage.cpp
 *  @brief    设置窗口系统设置界面类
 *  @author
 *  @email
 *  @version
 *  @date     2020.12.21
 *  Copyright (c) 2020 SmartMore All rights reserved.
 ****************************************************************************/
#include "systemsettingpage.h"
#include "configdatamanager/configmanager.h"
#include "ui_systemsettingpage.h"

#include <QDebug>
#include <QFile>

namespace vtk::display
{
using ConfigManager = vtk::config::ConfigManager;
using SystemConfigData = vtk::config::SystemConfigData;

SystemSettingPage::SystemSettingPage(QWidget *parent) : QWidget(parent), ui(new Ui::SystemSettingPage)
{
    ui->setupUi(this);
    ui->system_name_input->installEventFilter(this);
    ui->system_logo_title->hide();
    ui->system_logo_input->hide();
    ui->system_name_input->setText(tr("SMore VTK Operating Platform"));

    connect(ui->system_name_input, &QLineEdit::editingFinished, this, [=] { disableFocus(); });
}

SystemSettingPage::~SystemSettingPage()
{
    delete ui;
}

void SystemSettingPage::disableFocus()
{
    validateSystemName();
    ui->system_name_input->clearFocus();
}

bool SystemSettingPage::validateSystemName()
{
    if (ui->system_name_input->isEmpty())
    {
        ui->system_name_warning->setText(tr("Please enter the system name."));
        return false;
    }
    if (ui->system_name_input->exceedLength(30))
    {
        ui->system_name_warning->setText(tr("The name length is between 1 and 30 characters."));
        return false;
    }
    ui->system_name_warning->setText("");
    return true;
}

void SystemSettingPage::cancelSystemConfig()
{
    initSystemConfigData();
    ui->system_name_warning->setText("");
}

void SystemSettingPage::initSystemConfigData()
{
    const auto &system_config_data = ConfigManager::getInstance().getSystemConfigData();
    if (!system_config_data.app_name.isEmpty())
        ui->system_name_input->setText(system_config_data.app_name);
}

void SystemSettingPage::saveSystemConfigData()
{
    SystemConfigData data;
    data.app_name = ui->system_name_input->text();
    ConfigManager::getInstance().setSystemConfigData(data);
}

bool SystemSettingPage::eventFilter(QObject *wcg, QEvent *event)
{
    if (wcg == ui->system_name_input && event->type() == QEvent::FocusIn)
        ui->system_name_warning->setText("");
    return QWidget::eventFilter(wcg, event);
}
}  // namespace vtk::display
