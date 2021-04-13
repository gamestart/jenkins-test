/****************************************************************************
 *  @file     configdeserialization.h
 *  @brief    配置反序列化
 *  @author   xiaoyu.liu
 *  @email    xiaoyu.liu@smartmore.com
 *  @version
 *  @date     2020-12-22
 *  Copyright (c) 2020 SmartMore All rights reserved.
 ****************************************************************************/

#ifndef CONFIGDESERIALIZATION_H
#define CONFIGDESERIALIZATION_H

#include "configmanager.h"

namespace vtk::config
{

QDataStream &operator>>(QDataStream &stream, SystemConfigData &config);

QDataStream &operator>>(QDataStream &stream, CameraConfigItem &config);

QDataStream &operator>>(QDataStream &stream, AlgoConfigItem &config);

QDataStream &operator>>(QDataStream &stream, AlgoConfigData &config);

QDataStream &operator>>(QDataStream &stream, const SourceConfigItem &config);

QDataStream &operator>>(QDataStream &stream, ViewConfigItem &config);

QDataStream &operator>>(QDataStream &stream, SaveConfigItem &config);

QDataStream &operator>>(QDataStream &stream, SaveConfigData &config);

QDataStream &operator>>(QDataStream &stream, ConfigManager &config);

}  // namespace vtk::config

#endif  // CONFIGDESERIALIZATION_H
