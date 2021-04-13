/****************************************************************************
 *  @file     imagesourcepage.cpp
 *  @brief    图像数据源设置页面类
 *  @author   junjie.zeng
 *  @email    junjie.zeng@smartmore.com
 *  @version
 *  @date     2021.01.08
 *  Copyright (c) 2021 SmartMore All rights reserved.
 ****************************************************************************/
#include "imagesourcepage.h"
#include "imagesourcesettingwidget.h"
#include "ui_imagesourcepage.h"
#include "uiframe/commonuiset/messagedialog/messagedialog.h"
#include <QDialog>
#include <QListWidgetItem>

namespace vtk::display
{
#define TRIGGERLEVELCOUNT 5  //触发层级总个数
#define TRIGGERLEVELORDERCOUNT 10  //同一触发层级的执行顺序总个数
#define SOURCEWIDGETMAXSIZE 10  //数据源最大创建个数

using ConfigManager = vtk::config::ConfigManager;
using Message = vtk::common::Message;
using ViewConfigData = vtk::config::ViewConfigData;

ImageSourcePage::ImageSourcePage(QWidget *parent) : QWidget(parent), ui(new Ui::ImageSourcePage)
{
    init();
}

ImageSourcePage::~ImageSourcePage()
{
    delete ui;
}

void ImageSourcePage::initImageSourceConfigData()
{
    ui->image_source_widget->setAddButtonStatus(true);
    const auto &data_list = ConfigManager::getInstance().getCameraConfigData();
    foreach (const auto &data, data_list)
    {
        for (int i = 0; i < TRIGGERLEVELCOUNT; i++)
        {
            for (int j = 0; j < TRIGGERLEVELORDERCOUNT; j++)
                m_source_hash[data.name][QString("L%1").arg(i)].insert(QString("L%1-%2").arg(i).arg(j), false);
        }
    }
    const auto &source_data_list = ConfigManager::getInstance().getSourceConfigData();
    const auto view_counts = ConfigManager::getInstance().getViewCounts();
    int i{0};
    foreach (const auto &data, source_data_list)
    {
        if (i++ < view_counts)
            continue;

        addItem(data);
    }
}

void ImageSourcePage::saveImageSourceConfigData()
{
    const auto count = ui->image_source_widget->getListItemCounts();
    vtk::config::SourceConfigData datas;
    const auto view_counts = ConfigManager::getInstance().getViewCounts();
    const auto &source_data_list = ConfigManager::getInstance().getSourceConfigData();
    assert(source_data_list.size() >= view_counts);
    for (int i = 0; i < view_counts; i++)
        datas.append(source_data_list.at(i));
    for (int row = 0; row < count; row++)
    {
        auto source_widget = dynamic_cast<ImageSourceSettingWidget *>(ui->image_source_widget->getItemWidget(row));
        if (source_widget)
        {
            const auto data = source_widget->getSourceConfigData();
            datas.append(std::move(data));
        }
    }
    ConfigManager::getInstance().setSourceConfigData(datas);
}

void ImageSourcePage::cancelImageSourceConfig()
{
    ui->image_source_widget->clear();
    m_source_hash.clear();
    m_source_name_list.clear();
    m_trigger_char_list.clear();
    initImageSourceConfigData();
}

bool ImageSourcePage::validateImageSourceSettings()
{
    int flag = 0;
    for (int i = 0; i < ui->image_source_widget->getListItemCounts(); i++)
    {
        auto source_setting = dynamic_cast<ImageSourceSettingWidget *>(ui->image_source_widget->getItemWidget(i));
        if (!source_setting->validateSourceSetting())
            flag++;
    }
    if (flag)
        return false;
    else
        return true;
}

void ImageSourcePage::disableFocus()
{
    for (int i = 0; i < ui->image_source_widget->getListItemCounts(); i++)
    {
        auto source_setting = dynamic_cast<ImageSourceSettingWidget *>(ui->image_source_widget->getItemWidget(i));
        source_setting->disableFocus(1);
        source_setting->disableFocus(2);
    }
}

bool ImageSourcePage::slotCheckSourceName(const QString &old_source_name, const QString &new_source_name)
{
    if (m_source_name_list.contains(new_source_name))
        return false;
    m_source_name_list.removeOne(old_source_name);
    m_source_name_list.append(new_source_name);
    return true;
}

bool ImageSourcePage::slotCheckTriggerChar(const QString &old_char, const QString &new_char)
{
    if (m_trigger_char_list.contains(new_char))
        return false;
    m_trigger_char_list.removeOne(old_char);
    m_trigger_char_list.append(new_char);
    return true;
}

void ImageSourcePage::init()
{
    initWidgets();
    initConnects();
}

void ImageSourcePage::initWidgets()
{
    ui->setupUi(this);
    ui->image_source_widget->setAddButtonToolTip(tr("New Picture Source"));
}

void ImageSourcePage::initConnects()
{
    connect(ui->image_source_widget, &FactoryWidget::signalAddOne, this, [&] {
        if (ui->image_source_widget->getAddButtonStatus() != "enabled")
        {
            MessageDialog at_most_ten_source_dialog(DialogType::WarningDialog,
                                                    tr("The number of sources should be at most 10"));
            at_most_ten_source_dialog.exec();
            return;
        }
        SourceConfigItem data;
        const auto index = ui->image_source_widget->getListItemCounts();
        data.source_name = QString("Picture%1").arg(index + 1);
        const auto &data_list = ConfigManager::getInstance().getCameraConfigData();
        if (data_list.empty())
        {
            MessageDialog no_camera_dialog(DialogType::WarningDialog,
                                           tr("There is no camera in list, source setting is therefore disabled. You "
                                              "can set the view source to be \"File\" in view setting."));
            no_camera_dialog.exec();
            return;
        }
        data.source = data_list.first().name;
        data.trigger_char = QString("T%1").arg(index);
        data.trigger_level = "L0";
        auto iter = m_source_hash[data.source][data.trigger_level].begin();
        while (iter != m_source_hash[data.source][data.trigger_level].end())
        {
            if (iter.value())
            {
                iter++;
                continue;
            }
            data.trigger_level_order = iter.key();
            break;
        }
        if (iter == m_source_hash[data.source][data.trigger_level].end())
        {
            Message::postWarningMessage(tr("Trigger level order no value to use!"));
            return;
        }
        addItem(data);
    });
}

bool ImageSourcePage::checkSourceNameUsed(QString name)
{
    ViewConfigData view_data = ConfigManager::getInstance().getViewConfigData();
    for (const auto &data : view_data)
    {
        if (data.source == name)
            return true;
    }
    return false;
}

void ImageSourcePage::addItem(const SourceConfigItem &data)
{
    auto get_list = [&](const QString &source, const QString &level) -> QStringList {
        QStringList list;
        if (m_source_hash[source].contains(level))
        {
            auto iter = m_source_hash[source][level].begin();
            while (iter != m_source_hash[source][level].end())
            {
                if (iter.value())
                {
                    iter++;
                    continue;
                }
                list.append(iter.key());
                iter++;
            }
        }
        return list;
    };

    auto item = new QListWidgetItem;
    auto widget = new ImageSourceSettingWidget(data);
    m_source_hash[data.source][data.trigger_level][data.trigger_level_order] = true;
    m_source_name_list.append(data.source_name);
    m_trigger_char_list.append(data.trigger_char);
    connect(widget, &ImageSourceSettingWidget::signalSourceNameTextChanged, this,
            &ImageSourcePage::slotCheckSourceName);

    connect(
        widget, &ImageSourceSettingWidget::signalImageSourceTextChanged, this,
        [=](const QString &old_source, const QString &new_source) {
            const auto level = widget->getCurrentTriggerLevel();
            m_source_hash[old_source][level][widget->getCurrentTriggerLevelOrder()] = false;
            resetSameLevelOfTriggerLevelOrderCombox(old_source, level);
            widget->resetTriggerLevelOrderCombox(get_list(new_source, level), widget->getCurrentTriggerLevelOrder());
            m_source_hash[new_source][widget->getCurrentTriggerLevel()][widget->getCurrentTriggerLevelOrder()] = true;
            resetSameLevelOfTriggerLevelOrderCombox(new_source, level);
        });

    connect(widget, &ImageSourceSettingWidget::signalTriggerCharTextChanged, this,
            &ImageSourcePage::slotCheckTriggerChar);
    connect(widget, &ImageSourceSettingWidget::signalTriggerLevelTextChanged, this,
            [=](const QString &source, const QString &trigger_level) {
                widget->resetTriggerLevelOrderCombox(get_list(source, trigger_level));
            });

    connect(widget, &ImageSourceSettingWidget::signalTriggerLevelOrderTextChanged, this,
            [&](const QString &source, const QString &old_order, const QString &new_order) {
                const auto level_old = old_order.left(2);
                const auto level_new = new_order.left(2);
                if (m_source_hash[source].contains(level_old) && m_source_hash[source].contains(level_new))
                {
                    m_source_hash[source][level_old][old_order] = false;
                    m_source_hash[source][level_new][new_order] = true;
                    if (level_old != level_new)
                    {
                        resetSameLevelOfTriggerLevelOrderCombox(source, level_old);
                        resetSameLevelOfTriggerLevelOrderCombox(source, level_new);
                    }
                    else
                        resetSameLevelOfTriggerLevelOrderCombox(source, level_new);
                }
            });
    connect(widget, &ImageSourceSettingWidget::signalDelete, this, [=] {
        QString source_name = widget->getCurrentImageSourceName();
        if (checkSourceNameUsed(source_name))
        {
            MessageDialog source_name_used_dialog(
                DialogType::WarningDialog,
                tr("The source is being used. Delete this source will also delete the source selected in the "
                   "corresponding view. Still want to delete it?"),
                {Yes, No});
            int result = source_name_used_dialog.exec();
            if (result == QDialog::Rejected)
                return;
        }
        m_source_name_list.removeOne(source_name);
        m_trigger_char_list.removeOne(widget->getCurrentTriggerChar());
        const auto source = widget->getCurrentImageSource();
        const auto level = widget->getCurrentTriggerLevel();
        m_source_hash[source][level][widget->getCurrentTriggerLevelOrder()] = false;
        const auto row = ui->image_source_widget->getItemRow(item);
        ui->image_source_widget->deleteListItem(item);
        const auto count = ui->image_source_widget->getListItemCounts();
        ViewConfigData view_data = ConfigManager::getInstance().getViewConfigData();
        for (auto &data : view_data)
        {
            if (data.source == source_name)
            {
                data.source = "File";
            }
        }
        for (int i = row; i < count; i++)
        {
            auto source_widget = dynamic_cast<ImageSourceSettingWidget *>(ui->image_source_widget->getItemWidget(i));
            if (source_widget)
            {
                auto get_new_name = [&](const QString &personality_string, const QString &name,
                                        int num) -> const QString {
                    const auto default_name = personality_string + QString("%1").arg(num + 1);
                    const auto new_name = personality_string + QString("%1").arg(num);
                    if (name == default_name)
                    {
                        m_source_name_list.removeOne(default_name);
                        auto new_name_temp = new_name;
                        while (m_source_name_list.contains(new_name_temp))
                        {
                            static int j{1};
                            new_name_temp = new_name + QString("_%1").arg(j++);
                        }
                        return new_name_temp;
                    }
                    return name;
                };
                QString default_name = "Picture" + QString("%1").arg(i + 2);
                QString new_name = get_new_name("Picture", source_widget->getCurrentImageSourceName(), i + 1);
                source_widget->setCurrentImageSourceName(new_name);
                for (auto &data : view_data)
                {
                    if (data.source == default_name)
                    {
                        data.source = new_name;
                    }
                }
                source_widget->setCurrentTriggerChar(get_new_name("T", source_widget->getCurrentTriggerChar(), i));
            }
        }
        resetSameLevelOfTriggerLevelOrderCombox(source, level);
        if (ui->image_source_widget->getListItemCounts() < SOURCEWIDGETMAXSIZE)
            ui->image_source_widget->setAddButtonStatus(true);
        else
            ui->image_source_widget->setAddButtonStatus(false);
        ConfigManager::getInstance().setViewConfigData(view_data);
    });
    item->setSizeHint(QSize(280, 384));
    ui->image_source_widget->addListItem(item, widget);
    resetSameLevelOfTriggerLevelOrderCombox(data.source, data.trigger_level);
    if (ui->image_source_widget->getListItemCounts() >= SOURCEWIDGETMAXSIZE)
        ui->image_source_widget->setAddButtonStatus(false);
    else
        ui->image_source_widget->setAddButtonStatus(true);
}

void ImageSourcePage::resetSameLevelOfTriggerLevelOrderCombox(const QString &source, const QString &same_level)
{
    auto get_list = [&](const QString &level, const QString &level_order) -> QStringList {
        QStringList list;
        if (m_source_hash[source].contains(level))
        {
            auto iter = m_source_hash[source][level].begin();
            while (iter != m_source_hash[source][level].end())
            {
                if (iter.key() != level_order && iter.value())
                {
                    iter++;
                    continue;
                }
                list.append(iter.key());
                iter++;
            }
        }
        return list;
    };

    auto count = ui->image_source_widget->getListItemCounts();
    for (int row = 0; row < count; row++)
    {
        auto source_widget = dynamic_cast<ImageSourceSettingWidget *>(ui->image_source_widget->getItemWidget(row));
        if (source_widget && source == source_widget->getCurrentImageSource() &&
            source_widget->getCurrentTriggerLevel() == same_level)
            source_widget->resetTriggerLevelOrderCombox(
                get_list(same_level, source_widget->getCurrentTriggerLevelOrder()),
                source_widget->getCurrentTriggerLevelOrder());
    }
}

}  // namespace vtk::display
