/****************************************************************************
 *  @file     crashdumper.h
 *  @brief    异常退出dumper
 *  @author   xiaoyu.liu
 *  @email    xiaoyu.liu@smartmore.com
 *  @version
 *  @date     2021-01-08
 *  Copyright (c) 2021 SmartMore All rights reserved.
 ****************************************************************************/

#ifndef CRASHDUMPER_H
#define CRASHDUMPER_H

namespace vtk::common
{

/**
 * @brief registerCrashDumper
 * 异常退出时生成dump文件，在main函数中调用
 * windows下dump文件位置dumps/smore_vtk.dmp
 * linux下dump文件默认为程序目录下的core或参考系统配置
 * release下需要打开生成额外调试信息
 */
void registerCrashDumper();

}  // namespace vtk::common

#endif  // CRASHDUMPER_H
