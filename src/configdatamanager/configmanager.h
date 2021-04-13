/****************************************************************************
 *  @file     configmanager.h
 *  @brief    配置管理类
 *  @author   xiaoyu.liu
 *  @email    xiaoyu.liu@smartmore.com
 *  @version
 *  @date     2020-12-22
 *  Copyright (c) 2020 SmartMore All rights reserved.
 ****************************************************************************/

#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include "common/vtkcommon.h"
#include <QList>
#include <QObject>
#include <QString>
#include <QUuid>
#include <QVector>

namespace vtk::config
{

using CameraState = vtk::common::CameraState;
using AlgoType = vtk::common::AlgoType;

enum class TriggerMode : int
{
    softTrigger = 0,
    hardTrigger
};

struct CameraConfigItem
{
    QString name;
    QString id;
    CameraState state;
    QString ip;
    QString subnet_mask;
    QString gain;
    QString exposure;
    QString gamma;
    int trigger_source{-1};
    int trigger_activation{-1};
};

typedef QList<CameraConfigItem> CameraConfigData;

struct SourceConfigItem
{
    QString source_name;
    QString source;
    QString trigger_char;
    QString trigger_level;
    QString trigger_level_order;
    QUuid uuid{QUuid::createUuid()};
};

typedef QList<SourceConfigItem> SourceConfigData;

struct ViewConfigItem
{
    QUuid uuid{QUuid::createUuid()};
    QString name;
    QString source;
    QStringList algo;
};

typedef QList<ViewConfigItem> ViewConfigData;

struct AlgoConfigItem
{
    QUuid uuid{QUuid::createUuid()};
    QString name;
    AlgoType type;
    QString model_file;
};

struct AlgoConfigData
{
    bool gpu_mode{false};
    QVector<AlgoConfigItem> items;
};

struct SaveConfigItem
{
    QString ok_path;
    QString ng_path;
    QString pic_format;
    int save_number = 1000;
    int save_day = 30;
    bool save_pic = true;
    bool save_ok = true;
    bool save_ng = true;
    bool delete_pic = true;
};

struct SaveConfigData
{
    SaveConfigItem original;
    SaveConfigItem rendered;
};

struct SystemConfigData
{
    QString app_name;
};

class ConfigManager : public QObject
{
    Q_OBJECT

public:
    ~ConfigManager() = default;

    static bool loadFromFile();
    static bool saveToFile();
    static ConfigManager &getInstance();
    void init();

    const SystemConfigData &getSystemConfigData() const
    {
        return m_system_config_data;
    }

    const CameraConfigData &getCameraConfigData() const
    {
        return m_camera_config_data;
    }

    const AlgoConfigData &getAlgoConfigData() const
    {
        return m_algo_config_data;
    }

    const SourceConfigData &getSourceConfigData() const
    {
        return m_source_config_data;
    }

    const ViewConfigData &getViewConfigData() const
    {
        return m_view_config_data;
    }

    const SaveConfigData &getImageSaveConfigData() const
    {
        return m_image_save_config_data;
    }

    TriggerMode getTriggerMode() const
    {
        return m_trigger_mode;
    }

    void setSystemConfigData(const SystemConfigData &config)
    {
        m_system_config_data = std::move(config);
        emit signalSystemConfigDataChanged();
    }

    void setCameraConfigData(const CameraConfigData &config)
    {
        m_camera_config_data = std::move(config);
        emit signalCameraConfigDataChanged();
    }

    void setAlgoConfigData(const AlgoConfigData &config)
    {
        m_algo_config_data = std::move(config);
        emit signalAlgoConfigDataChanged();
    }

    void setSourceConfigData(const SourceConfigData &config)
    {
        m_source_config_data = std::move(config);
        emit signalSourceConfigDataChanged();
    }

    void setViewConfigData(const ViewConfigData &config)
    {
        m_view_config_data = std::move(config);
        emit signalViewConfigDataChanged();
    }

    void setImageSaveConfigData(const SaveConfigData &config)
    {
        m_image_save_config_data = std::move(config);
        emit signalImageSaveConfigDataChanged();
    }

    void setTriggerMode(TriggerMode trigger_mode)
    {
        m_trigger_mode = trigger_mode;
        emit signalTriggerModeChanged();
    }

    void addPredefinedSourceConfigData(int view_pos);
    void deletePredefinedSourceConfigData(int view_pos);
    inline int getViewCounts() const
    {
        return m_view_counts;
    }

signals:
    void signalConfigChanged();
    void signalSystemConfigDataChanged();
    void signalCameraConfigDataChanged();
    void signalAlgoConfigDataChanged();
    void signalSourceConfigDataChanged();
    void signalViewConfigDataChanged();
    void signalImageSaveConfigDataChanged();
    void signalTriggerModeChanged();

private:
    ConfigManager() = default;
    DISALLOW_COPY_AND_ASSIGN(ConfigManager)

    void checkSystemConfigData();
    void checkSourceConfigData();
    void checkViewConfigData();
    void checkSaveConfigData();

    friend QDataStream &operator<<(QDataStream &stream, const ConfigManager &config);
    friend QDataStream &operator>>(QDataStream &stream, ConfigManager &config);

private:
    static QString sm_config_major_version;
    SystemConfigData m_system_config_data;
    CameraConfigData m_camera_config_data;
    AlgoConfigData m_algo_config_data;
    SourceConfigData m_source_config_data;
    ViewConfigData m_view_config_data;
    SaveConfigData m_image_save_config_data;
    int m_view_counts{1};
    TriggerMode m_trigger_mode{TriggerMode::softTrigger};
};

}  // namespace vtk::config

Q_DECLARE_METATYPE(vtk::config::TriggerMode);
#endif  // CONFIGMANAGER_H
