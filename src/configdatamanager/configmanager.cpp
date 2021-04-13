/****************************************************************************
 *  @file     configmanager.cpp
 *  @brief    配置管理类
 *  @author   xiaoyu.liu
 *  @email    xiaoyu.liu@smartmore.com
 *  @version
 *  @date     2020-12-22
 *  Copyright (c) 2020 SmartMore All rights reserved.
 ****************************************************************************/

#include "configmanager.h"
#include "configdeserialization.h"
#include "configserialization.h"
#include <QDataStream>
#include <QFile>

namespace vtk::config
{

const QString c_config_full_name = "settings.dat";
QString ConfigManager::sm_config_major_version = "1";

ConfigManager &ConfigManager::getInstance()
{
    static ConfigManager s_config_manager;
    return s_config_manager;
}

void ConfigManager::init()
{
    connect(this, &ConfigManager::signalViewConfigDataChanged, this, [&] {
        const auto new_counts = m_view_config_data.size();
        const auto value = new_counts - m_view_counts;
        if (value > 0)
        {
            for (int i = m_view_counts; i < new_counts; i++)
                addPredefinedSourceConfigData(i);
        }
        else if (value < 0)
        {
            for (int i = 0; i < abs(value); i++)
                deletePredefinedSourceConfigData(new_counts);
        }
        m_view_counts = new_counts;
    });
    ConfigManager::loadFromFile();
}

void ConfigManager::checkSystemConfigData()
{
    if (m_system_config_data.app_name.isEmpty())
        m_system_config_data.app_name = tr("SMore VTK Operating Platform");
}

void ConfigManager::checkSourceConfigData()
{
    if (m_source_config_data.isEmpty())
        addPredefinedSourceConfigData(0);
}

void ConfigManager::checkViewConfigData()
{
    if (m_view_config_data.empty())
    {
        m_view_config_data.push_back({QUuid::createUuid(), "View1", "File", {}});
    }
}

void ConfigManager::checkSaveConfigData()
{
    auto base_dir = qApp->applicationDirPath() + "/data";
    if (m_image_save_config_data.original.ok_path.isEmpty())
        m_image_save_config_data.original.ok_path = base_dir;

    if (m_image_save_config_data.original.ng_path.isEmpty())
        m_image_save_config_data.original.ng_path = base_dir;

    if (m_image_save_config_data.original.pic_format.isEmpty())
        m_image_save_config_data.original.pic_format = "jpg";

    if (m_image_save_config_data.rendered.ok_path.isEmpty())
        m_image_save_config_data.rendered.ok_path = base_dir;

    if (m_image_save_config_data.rendered.ng_path.isEmpty())
        m_image_save_config_data.rendered.ng_path = base_dir;

    if (m_image_save_config_data.rendered.pic_format.isEmpty())
        m_image_save_config_data.rendered.pic_format = "jpg";
}

bool ConfigManager::loadFromFile()
{
    const auto camera_data_list = ConfigManager::getInstance().m_camera_config_data;
    ConfigManager::getInstance().m_camera_config_data.clear();
    ConfigManager::getInstance().m_source_config_data.clear();
    ConfigManager::getInstance().m_view_config_data.clear();

    bool ret{false};
    //尝试从文件加载
    QFile file(c_config_full_name);
    if (file.exists())
    {
        if (file.open(QIODevice::ReadOnly))
        {
            QDataStream stream(&file);
            stream >> ConfigManager::getInstance();
            file.close();
            ret = true;
        }
    }
    else
    {
        //初始化默认值
        ConfigManager::getInstance().checkSystemConfigData();
        ConfigManager::getInstance().checkSourceConfigData();
        ConfigManager::getInstance().checkViewConfigData();
        ConfigManager::getInstance().checkSaveConfigData();
        saveToFile();
    }

    if (ConfigManager::getInstance().m_camera_config_data.isEmpty())
    {
        ConfigManager::getInstance().setCameraConfigData(camera_data_list);
    }
    ConfigManager::getInstance().m_view_counts = ConfigManager::getInstance().m_view_config_data.size();
    return ret;
}

bool ConfigManager::saveToFile()
{
    bool ret{false};
    QFile file(c_config_full_name);
    if (file.open(QIODevice::ReadWrite))
    {
        QDataStream stream(&file);
        stream << ConfigManager::getInstance();
        file.close();
        ret = true;
    }
    emit ConfigManager::getInstance().signalConfigChanged();
    return ret;
}

void ConfigManager::addPredefinedSourceConfigData(int view_pos)
{
    // view_pos 从0值开始
    SourceConfigItem data;
    data.source_name = QString("LocalSource%1").arg(view_pos);
    data.source = QString("Local%1").arg(view_pos);
    data.trigger_char = QString("LT%1").arg(view_pos);
    data.trigger_level = "L0";
    data.trigger_level_order = "L0-0";
    m_source_config_data.insert(view_pos, data);
}

void ConfigManager::deletePredefinedSourceConfigData(int view_pos)
{
    // view_pos 从0值开始
    m_source_config_data.removeAt(view_pos);
}

}  // namespace vtk::config
