/****************************************************************************
 *  @file     asyncimagesaver.h
 *  @brief    异步图像保存类
 *  @author   xiaoyu.liu
 *  @email    xiaoyu.liu@smartmore.com
 *  @version
 *  @date     2020-12-22
 *  Copyright (c) 2020 SmartMore All rights reserved.
 ****************************************************************************/

#ifndef ASYNCIMAGESAVER_H
#define ASYNCIMAGESAVER_H

#include "common/vtkcommon.h"
#include <QList>
#include <QString>

namespace vtk::imagesave
{

struct ImageInfo
{
    bool is_src_image{true};
    QString image_full_name;
    cv::Mat mat;
};

class AsyncImageSaver
{
public:
    static void startTasks();
    static void saveResultImages(int view_index, const vtk::common::AlgoResult &algo_result,
                                 bool is_local_image = false);
    static void addTasks(const QList<ImageInfo> &image_info);
    static void waitQuit();
};

}  // namespace vtk::imagesave
#endif  // ASYNCIMAGESAVER_H
