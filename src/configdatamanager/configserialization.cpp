/****************************************************************************
 *  @file     configserialization.cpp
 *  @brief    配置序列化
 *  @author   xiaoyu.liu
 *  @email    xiaoyu.liu@smartmore.com
 *  @version
 *  @date     2020-12-22
 *  Copyright (c) 2020 SmartMore All rights reserved.
 ****************************************************************************/

#include "configserialization.h"
#include <QByteArray>
#include <QDataStream>
#include <QVariant>

namespace vtk::config
{

class SerializationHelper
{
public:
    SerializationHelper(QDataStream &stream)
        : m_member_size(0), m_byte_array(), m_stream(&m_byte_array, QIODevice::ReadWrite), m_out_stream(stream)
    {
    }

    ~SerializationHelper()
    {
        m_out_stream << m_member_size;
        m_out_stream.writeRawData(m_byte_array.data(), m_byte_array.size());
    }

    void add(const QString &key, QVariant value)
    {
        m_stream << key << value;
        m_member_size++;
    }

    void add(const QString &key, size_t value)
    {
        m_stream << key << QVariant((qulonglong)value);
        m_member_size++;
    }

    template <class T> void addStruct(const QString &key, const T &value)
    {
        QVariant var;
        m_stream << key << var << value;
        m_member_size++;
    }

    void addStruct(const QString &key, const QString &value) = delete;
    void addStruct(const QString &key, const bool &value) = delete;
    void addStruct(const QString &key, const int &value) = delete;

private:
    uint32_t m_member_size;
    QByteArray m_byte_array;
    QDataStream m_stream;
    QDataStream &m_out_stream;
};

#define MEMBER(member) #member, config.member

QDataStream &operator<<(QDataStream &stream, const SystemConfigData &config)
{
    SerializationHelper helper(stream);

    helper.add(MEMBER(app_name));

    return stream;
}

QDataStream &operator<<(QDataStream &stream, const CameraConfigItem &config)
{
    SerializationHelper helper(stream);

    helper.add(MEMBER(name));
    helper.add(MEMBER(id));
    helper.add("state", static_cast<int>(config.state));
    helper.add(MEMBER(ip));
    helper.add(MEMBER(subnet_mask));
    helper.add(MEMBER(gain));
    helper.add(MEMBER(exposure));
    helper.add(MEMBER(gamma));
    helper.add(MEMBER(trigger_source));
    helper.add(MEMBER(trigger_activation));

    return stream;
}

QDataStream &operator<<(QDataStream &stream, const AlgoConfigItem &config)
{
    SerializationHelper helper(stream);

    helper.add(MEMBER(uuid));
    helper.add(MEMBER(name));
    helper.add("type", static_cast<int>(config.type));
    helper.add(MEMBER(model_file));

    return stream;
}

QDataStream &operator<<(QDataStream &stream, const AlgoConfigData &config)
{
    SerializationHelper helper(stream);

    helper.add(MEMBER(gpu_mode));
    helper.addStruct(MEMBER(items));

    return stream;
}

QDataStream &operator<<(QDataStream &stream, const SourceConfigItem &config)
{
    SerializationHelper helper(stream);

    helper.add(MEMBER(source_name));
    helper.add(MEMBER(source));
    helper.add(MEMBER(trigger_char));
    helper.add(MEMBER(trigger_level));
    helper.add(MEMBER(trigger_level_order));
    helper.add(MEMBER(uuid));

    return stream;
}

QDataStream &operator<<(QDataStream &stream, const ViewConfigItem &config)
{
    SerializationHelper helper(stream);

    helper.add(MEMBER(uuid));
    helper.add(MEMBER(name));
    helper.add(MEMBER(source));
    helper.add(MEMBER(algo));

    return stream;
}

QDataStream &operator<<(QDataStream &stream, const SaveConfigItem &config)
{
    SerializationHelper helper(stream);

    helper.add(MEMBER(ok_path));
    helper.add(MEMBER(ng_path));
    helper.add(MEMBER(pic_format));
    helper.add(MEMBER(save_number));
    helper.add(MEMBER(save_day));
    helper.add(MEMBER(save_pic));
    helper.add(MEMBER(save_ok));
    helper.add(MEMBER(save_ng));
    helper.add(MEMBER(delete_pic));

    return stream;
}

QDataStream &operator<<(QDataStream &stream, const SaveConfigData &config)
{
    SerializationHelper helper(stream);

    helper.addStruct(MEMBER(original));
    helper.addStruct(MEMBER(rendered));

    return stream;
}

QDataStream &operator<<(QDataStream &stream, const ConfigManager &config)
{
    SerializationHelper helper(stream);

    helper.add("config_major_version", config.sm_config_major_version);

    helper.addStruct("system", config.getSystemConfigData());
    helper.addStruct("camera", config.getCameraConfigData());
    helper.addStruct("algo", config.getAlgoConfigData());
    helper.addStruct("source", config.getSourceConfigData());
    helper.addStruct("view", config.getViewConfigData());
    helper.addStruct("save", config.getImageSaveConfigData());
    helper.add("trigger_mode", static_cast<int>(config.m_trigger_mode));

    return stream;
}

}  // namespace vtk::config
