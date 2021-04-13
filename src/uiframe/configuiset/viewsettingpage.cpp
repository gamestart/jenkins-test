/****************************************************************************
 *  @file     viewsettingpage.cpp
 *  @brief    设置窗口显示设置界面类
 *  @author
 *  @email
 *  @version
 *  @date     2020.12.21
 *  Copyright (c) 2020 SmartMore All rights reserved.
 ****************************************************************************/
#include "viewsettingpage.h"
#include "ui_viewsettingpage.h"
#include "uiframe/commonuiset/messagedialog/messagedialog.h"

#include <QDebug>
#include <QFile>

namespace vtk::display
{
using ConfigManager = vtk::config::ConfigManager;
using ViewConfigData = vtk::config::ViewConfigData;
using ViewConfigItem = vtk::config::ViewConfigItem;

ViewSettingPage::ViewSettingPage(QWidget *parent) : QWidget(parent), ui(new Ui::ViewSettingPage)
{
    ui->setupUi(this);
    ui->view_setting_container->setAddButtonToolTip(tr("New View"));
    connect(ui->view_setting_container, &FactoryWidget::signalAddOne, this, [&] {
        ViewConfigItem data;
        data.name = "";
        data.algo = QStringList({});
        data.source = "";
        data.uuid = QUuid::createUuid();
        addView(data);
        assert(ui->view_setting_container->getListItemCounts() > 1);
        auto view_setting = dynamic_cast<ViewSetting *>(ui->view_setting_container->getItemWidget(0));
        view_setting->enableDeleteBtn(true);
    });
}

ViewSettingPage::~ViewSettingPage()
{
    delete ui;
}

void ViewSettingPage::updateViewSources()
{
    int count = ui->view_setting_container->getListItemCounts();
    for (int i = 0; i < count; i++)
    {
        auto view_setting = dynamic_cast<ViewSetting *>(ui->view_setting_container->getItemWidget(i));
        view_setting->updateViewSources();
    }
}

void ViewSettingPage::updateViewModels()
{
    for (int i = 0; i < ui->view_setting_container->getListItemCounts(); i++)
    {
        auto view_setting = dynamic_cast<ViewSetting *>(ui->view_setting_container->getItemWidget(i));
        view_setting->updateViewModels();
    }
}

void ViewSettingPage::addView(const ViewConfigItem data)
{
    if (ui->view_setting_container->getAddButtonStatus() == "enabled")
    {
        auto item = new QListWidgetItem;
        auto new_name = data.name;
        if (data.name.isEmpty())
        {
            new_name = "View" + QString::number(ui->view_setting_container->getListItemCounts() + 1);
        }
        auto widget = new ViewSetting(data.uuid, new_name, data.source, data.algo);
        connect(widget, &ViewSetting::signalDeleteView, this, [=] {
            const auto row = ui->view_setting_container->getItemRow(item);
            ui->view_setting_container->deleteListItem(item);
            const auto count = ui->view_setting_container->getListItemCounts();
            for (int i = row; i < count; i++)
            {
                auto view_setting = dynamic_cast<ViewSetting *>(ui->view_setting_container->getItemWidget(i));
                if (view_setting)
                {
                    auto get_new_name = [&](const QString &personality_string, const QString &name) -> const QString {
                        const auto default_name = personality_string + QString("%1").arg(i + 2);
                        const auto new_name = personality_string + QString("%1").arg(i + 1);
                        if (name == default_name)
                        {
                            return new_name;
                        }
                        return name;
                    };
                    view_setting->setViewName(get_new_name("View", view_setting->getViewName()));
                }
            }
            ui->view_setting_container->setAddButtonStatus(true);
            if (count == 1)
            {
                auto view_setting = dynamic_cast<ViewSetting *>(ui->view_setting_container->getItemWidget(0));
                if (view_setting)
                    view_setting->enableDeleteBtn(false);
            }
        });
        connect(widget, &ViewSetting::signalCheckDuplicate, this, [=] {
            const auto row = ui->view_setting_container->getItemRow(item);
            for (int i = 0; i < ui->view_setting_container->getListItemCounts(); i++)
            {
                auto view_setting = dynamic_cast<ViewSetting *>(ui->view_setting_container->getItemWidget(i));
                if (i == row)
                    continue;
                if (view_setting->getViewName() == widget->getViewName())
                    return true;
            }
            return false;
        });
        item->setSizeHint(QSize(280, 276));
        ui->view_setting_container->addListItem(item, widget);
        widget->enableDeleteBtn(true);
        updateViewSources();

        // 如果到十个视图则不允许添加
        if (ui->view_setting_container->getListItemCounts() == 10)
            ui->view_setting_container->setAddButtonStatus(false);
        else
            ui->view_setting_container->setAddButtonStatus(true);
    }
    else
    {
        MessageDialog at_most_ten_view_dialog(DialogType::WarningDialog,
                                              tr("The number of views should be at most 10"));
        at_most_ten_view_dialog.exec();
    }
}

void ViewSettingPage::cancelViewConfig()
{
    ui->view_setting_container->clear();
    initViewConfigData();
}

void ViewSettingPage::initViewConfigData()
{
    ui->view_setting_container->setAddButtonStatus(true);
    const auto &view_config_data = ConfigManager::getInstance().getViewConfigData();
    for (const auto &data : view_config_data)
    {
        addView(data);
    }
    if (view_config_data.size() == 1)
    {
        auto view_setting = dynamic_cast<ViewSetting *>(ui->view_setting_container->getItemWidget(0));
        view_setting->enableDeleteBtn(false);
    }
}

void ViewSettingPage::saveViewConfigData()
{
    ViewConfigData data;
    const auto count = ui->view_setting_container->getListItemCounts();
    for (int row = 0; row < count; row++)
    {
        auto view_setting = dynamic_cast<ViewSetting *>(ui->view_setting_container->getItemWidget(row));
        if (view_setting)
        {
            ViewConfigItem data_item;
            data_item.uuid = view_setting->getUuid();
            data_item.name = view_setting->getViewName();
            data_item.source = view_setting->getViewSource();
            data_item.algo = view_setting->getViewAlgo();
            data.push_back(data_item);
        }
    }
    ConfigManager::getInstance().setViewConfigData(data);
}

bool ViewSettingPage::validateViewSettings()
{
    int flag = 0;
    for (int i = 0; i < ui->view_setting_container->getListItemCounts(); i++)
    {
        auto view_setting = dynamic_cast<ViewSetting *>(ui->view_setting_container->getItemWidget(i));
        if (!view_setting->validateViewSetting())
            flag++;
    }
    if (flag)
        return false;
    else
        return true;
}

void ViewSettingPage::disableFocus()
{
    for (int i = 0; i < ui->view_setting_container->getListItemCounts(); i++)
    {
        auto view_setting = dynamic_cast<ViewSetting *>(ui->view_setting_container->getItemWidget(i));
        view_setting->disableFocus();
    }
}
}  // namespace vtk::display
