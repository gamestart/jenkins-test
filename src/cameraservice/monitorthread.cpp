/****************************************************************************
 *  @file     monitorthread.cpp
 *  @brief    相机设备监视线程类，监视相机是否掉线
 *  @author   junjie.zeng
 *  @email    junjie.zeng@smartmore.com
 *  @version
 *  @date     2020.12.29
 *  Copyright (c) 2020 SmartMore All rights reserved.
 ****************************************************************************/
#include "monitorthread.h"
#include "cameramanager.h"
#include "common/vtkcommon.h"
#include <QDebug>
#include <QMutexLocker>
#include <QProcess>

namespace vtk::cameraservice
{

MonitorThread::MonitorThread() {}

MonitorThread::~MonitorThread()
{
    stopMonitor();
}

void MonitorThread::setCameraIdList(std::vector<std::string> list)
{
    QMutexLocker lock(&m_mutex);
    m_camera_id_list = std::move(list);
}

void MonitorThread::stopMonitor()
{
    m_stop = true;
    if (isRunning())
    {
        wait();
        quit();
    }
}

void MonitorThread::run()
{
    m_stop = false;
    while (!m_stop)
    {
        {
            QMutexLocker lock(&m_mutex);
            auto camera_id_list = std::move(CaptureImage::getCameraInfoList());
            for (const auto &camera_id : camera_id_list)
            {
                auto exist = std::find(m_camera_id_list.begin(), m_camera_id_list.end(), camera_id);
                if (exist != m_camera_id_list.end())
                {
                    m_camera_id_list.erase(exist);
                }
            }
            for (auto &camera_id : m_camera_id_list)
            {
                LOGI(CameraManager, "camera %s is disconnected!", camera_id.c_str());
                emit signalCameraDisConnected(camera_id);
            }
            m_camera_id_list = std::move(camera_id_list);
        }
        sleep(5);
    }
}
}  // namespace vtk::cameraservice
