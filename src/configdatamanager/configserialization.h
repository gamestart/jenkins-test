/****************************************************************************
 *  @file     configserialization.h
 *  @brief    配置序列化
 *  @author   xiaoyu.liu
 *  @email    xiaoyu.liu@smartmore.com
 *  @version
 *  @date     2020-12-22
 *  Copyright (c) 2020 SmartMore All rights reserved.
 ****************************************************************************/

#ifndef CONFIGSERIALIZATION_H
#define CONFIGSERIALIZATION_H

#include "configmanager.h"

namespace vtk::config
{

QDataStream &operator<<(QDataStream &stream, const SystemConfigData &config);

QDataStream &operator<<(QDataStream &stream, const CameraConfigItem &config);

QDataStream &operator<<(QDataStream &stream, const AlgoConfigItem &config);

QDataStream &operator<<(QDataStream &stream, const AlgoConfigData &config);

QDataStream &operator<<(QDataStream &stream, const SourceConfigItem &config);

QDataStream &operator<<(QDataStream &stream, const ViewConfigItem &config);

QDataStream &operator<<(QDataStream &stream, const SaveConfigItem &config);

QDataStream &operator<<(QDataStream &stream, const SaveConfigData &config);

QDataStream &operator<<(QDataStream &stream, const ConfigManager &config);

}  // namespace vtk::config

#endif  // CONFIGSERIALIZATION_H
