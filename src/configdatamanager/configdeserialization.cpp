/****************************************************************************
 *  @file     configdeserialization.cpp
 *  @brief    配置反序列化
 *  @author   xiaoyu.liu
 *  @email    xiaoyu.liu@smartmore.com
 *  @version
 *  @date     2020-12-22
 *  Copyright (c) 2020 SmartMore All rights reserved.
 ****************************************************************************/

#include "configdeserialization.h"
#include <QDataStream>
#include <QVariant>
#include <vector>

namespace vtk::config
{

class DeserializationHelper
{
public:
    DeserializationHelper(QDataStream &stream) : m_member_size(0), m_stream(stream)
    {
        stream >> m_member_size;
    }

    ~DeserializationHelper() {}

    bool empty()
    {
        return m_member_size == 0;
    }

    QString getKey()
    {
        if (m_member_size == 0)
        {
            return nullptr;
        }
        QString key;
        m_stream >> key;
        m_member_size--;
        return key;
    }

    QVariant getValue()
    {
        QVariant value;
        m_stream >> value;
        return value;
    }

    void add(QString key, std::function<void(const QVariant &var)> func)
    {
        m_keys.push_back(key);
        m_funcs.push_back(func);
    }

    int process()
    {
        while (!empty())
        {
            auto key = getKey();
            auto it = std::find(m_keys.begin(), m_keys.end(), key);
            if (it != m_keys.end())
            {
                auto index = std::distance(m_keys.begin(), it);
                if (index >= 0 && index <= m_keys.size())
                {
                    auto var = getValue();
                    m_funcs[index](var);
                }
                else
                {
                    getValue();
                }
            }
        }
        return 0;
    }

private:
    uint32_t m_member_size;
    QDataStream &m_stream;

    std::vector<QString> m_keys;
    std::vector<std::function<void(const QVariant &var)>> m_funcs;
};

QDataStream &operator>>(QDataStream &stream, SystemConfigData &config)
{
    DeserializationHelper helper(stream);

    helper.add("app_name", [&](const QVariant &var) { config.app_name = var.toString(); });
    helper.process();

    return stream;
}

QDataStream &operator>>(QDataStream &stream, CameraConfigItem &config)
{
    DeserializationHelper helper(stream);

    helper.add("name", [&](const QVariant &var) { config.name = var.toString(); });
    helper.add("id", [&](const QVariant &var) { config.id = var.toString(); });
    helper.add("state", [&](const QVariant &var) { config.state = (CameraState)var.toInt(); });
    helper.add("ip", [&](const QVariant &var) { config.ip = var.toString(); });
    helper.add("subnet_mask", [&](const QVariant &var) { config.subnet_mask = var.toString(); });
    helper.add("gain", [&](const QVariant &var) { config.gain = var.toString(); });
    helper.add("exposure", [&](const QVariant &var) { config.exposure = var.toString(); });
    helper.add("gamma", [&](const QVariant &var) { config.gamma = var.toString(); });
    helper.add("trigger_source", [&](const QVariant &var) { config.trigger_source = var.toInt(); });
    helper.add("trigger_activation", [&](const QVariant &var) { config.trigger_activation = var.toInt(); });
    helper.process();

    return stream;
}

QDataStream &operator>>(QDataStream &stream, AlgoConfigItem &config)
{
    DeserializationHelper helper(stream);

    helper.add("uuid", [&](const QVariant &var) { config.uuid = var.toUuid(); });
    helper.add("name", [&](const QVariant &var) { config.name = var.toString(); });
    helper.add("type", [&](const QVariant &var) { config.type = (AlgoType)var.toInt(); });
    helper.add("model_file", [&](const QVariant &var) { config.model_file = var.toString(); });
    helper.process();

    return stream;
}

QDataStream &operator>>(QDataStream &stream, AlgoConfigData &config)
{
    DeserializationHelper helper(stream);

    helper.add("gpu_mode", [&](const QVariant &var) { config.gpu_mode = var.toBool(); });
    helper.add("items", [&](const QVariant &) { stream >> config.items; });
    helper.process();

    return stream;
}

QDataStream &operator>>(QDataStream &stream, SourceConfigItem &config)
{
    DeserializationHelper helper(stream);

    helper.add("source_name", [&](const QVariant &var) { config.source_name = var.toString(); });
    helper.add("source", [&](const QVariant &var) { config.source = var.toString(); });
    helper.add("trigger_char", [&](const QVariant &var) { config.trigger_char = var.toString(); });
    helper.add("trigger_level", [&](const QVariant &var) { config.trigger_level = var.toString(); });
    helper.add("trigger_level_order", [&](const QVariant &var) { config.trigger_level_order = var.toString(); });
    helper.add("uuid", [&](const QVariant &var) { config.uuid = var.toUuid(); });
    helper.process();

    return stream;
}

QDataStream &operator>>(QDataStream &stream, ViewConfigItem &config)
{
    DeserializationHelper helper(stream);

    helper.add("uuid", [&](const QVariant &var) { config.uuid = var.toUuid(); });
    helper.add("name", [&](const QVariant &var) { config.name = var.toString(); });
    helper.add("source", [&](const QVariant &var) { config.source = var.toString(); });
    helper.add("algo", [&](const QVariant &var) { config.algo = var.toStringList(); });
    helper.process();

    return stream;
}

QDataStream &operator>>(QDataStream &stream, SaveConfigItem &config)
{
    DeserializationHelper helper(stream);

    helper.add("ok_path", [&](const QVariant &var) { config.ok_path = var.toString(); });
    helper.add("ng_path", [&](const QVariant &var) { config.ng_path = var.toString(); });
    helper.add("pic_format", [&](const QVariant &var) { config.pic_format = var.toString(); });
    helper.add("save_number", [&](const QVariant &var) { config.save_number = var.toInt(); });
    helper.add("save_day", [&](const QVariant &var) { config.save_day = var.toInt(); });
    helper.add("save_pic", [&](const QVariant &var) { config.save_pic = var.toBool(); });
    helper.add("save_ok", [&](const QVariant &var) { config.save_ok = var.toBool(); });
    helper.add("save_ng", [&](const QVariant &var) { config.save_ng = var.toBool(); });
    helper.add("delete_pic", [&](const QVariant &var) { config.delete_pic = var.toBool(); });
    helper.process();

    return stream;
}

QDataStream &operator>>(QDataStream &stream, SaveConfigData &config)
{
    DeserializationHelper helper(stream);

    helper.add("original", [&](const QVariant &) { stream >> config.original; });
    helper.add("rendered", [&](const QVariant &) { stream >> config.rendered; });
    helper.process();

    return stream;
}

QDataStream &operator>>(QDataStream &stream, ConfigManager &config)
{
    DeserializationHelper helper(stream);

    auto key = helper.getKey();
    if (key != "config_major_version")
    {
        return stream;
    }
    auto major_version = helper.getValue().toString();

    if (major_version == config.sm_config_major_version)
    {
        helper.add("system", [&](const QVariant &) { stream >> config.m_system_config_data; });
        helper.add("camera", [&](const QVariant &) { stream >> config.m_camera_config_data; });
        helper.add("algo", [&](const QVariant &) { stream >> config.m_algo_config_data; });
        helper.add("source", [&](const QVariant &) { stream >> config.m_source_config_data; });
        helper.add("view", [&](const QVariant &) { stream >> config.m_view_config_data; });
        helper.add("save", [&](const QVariant &) { stream >> config.m_image_save_config_data; });
        helper.add("trigger_mode", [&](const QVariant &var) {
            config.m_trigger_mode = static_cast<vtk::config::TriggerMode>(var.toInt());
        });
        helper.process();
    }

    return stream;
}

}  // namespace vtk::config
