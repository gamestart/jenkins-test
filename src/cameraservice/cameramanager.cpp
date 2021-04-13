/****************************************************************************
 *  @file     cameramanager.cpp
 *  @brief    相机设备管理员
 *  @author   junjie.zeng
 *  @email    junjie.zeng@smartmore.com
 *  @version
 *  @date     2020.10.18
 *  Copyright (c) 2020 SmartMore All rights reserved.
 ****************************************************************************/

#include "cameramanager.h"
#include "cameradevice.h"
#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QMutexLocker>

namespace vtk::cameraservice
{

using CameraState = vtk::common::CameraState;

CameraManager &CameraManager::getInstance()
{
    static CameraManager s_cameras_manager;
    return s_cameras_manager;
}

CameraManager::~CameraManager()
{
    clearCameras();
}

void CameraManager::stopCamerasCapture()
{
    for (auto &capture : m_capture_map)
    {
        if (capture.second)
            capture.second->stopCapture();
    }
}

void CameraManager::startCameraCapture(const std::string &camera_id)
{
    if (m_capture_map.find(camera_id) != m_capture_map.end())
    {
        auto &capture = m_capture_map.at(camera_id);
        if (capture)
        {
            capture->startCapture();
        }
    }
    else
        LOGW(CameraManager, "Camera name: %s does not exist!", camera_id.c_str());
}

bool CameraManager::startCameraCapture(const std::string &camera_id, const QList<QString> &trigger_char_list)
{
    if (m_capture_map.find(camera_id) != m_capture_map.end())
    {
        auto &capture = m_capture_map.at(camera_id);
        if (capture)
            return capture->startCapture(trigger_char_list);
    }
    else
        LOGW(CameraManager, "Camera name: %s does not exist!", camera_id.c_str());
    return false;
}

void CameraManager::stopCameraCapture(const std::string &camera_id)
{
    if (m_capture_map.find(camera_id) != m_capture_map.end())
    {
        auto &capture = m_capture_map.at(camera_id);
        if (capture)
            capture->stopCapture();
    }
    else
        LOGW(CameraManager, "Camera name: %s does not exist!", camera_id.c_str());
}

void CameraManager::clearCameras()
{
    m_camera_id_list.clear();
    m_src_images_buff_map.clear();
    deleteAllCameras();
    if (m_monitor_thread)
    {
        m_monitor_thread->stopMonitor();
        m_monitor_thread = nullptr;
    }
}

void CameraManager::deleteDisconnectedCamera(const std::string &camera_id)
{
    for (auto it = m_disconnect_camera_id_list.begin(); it != m_disconnect_camera_id_list.end(); it++)
    {
        if (*it == camera_id)
        {
            m_disconnect_camera_id_list.erase(it);
            break;
        }
    }
}

void CameraManager::init()
{
    LOGI(CameraManager, "init camera!");

    if (m_first_init)
    {
        m_monitor_thread = std::make_unique<MonitorThread>();
        m_monitor_thread->start();
        connect(m_monitor_thread.get(), &MonitorThread::signalCameraDisConnected, this, [&](std::string camera_id) {
            const auto iter = m_capture_map.find(camera_id);
            if (iter != m_capture_map.end())
            {
                m_capture_map.erase(camera_id);
            }
            for (auto it = m_camera_id_list.begin(); it != m_camera_id_list.end(); it++)
            {
                if (*it == camera_id)
                {
                    m_camera_id_list.erase(it);
                    break;
                }
            }
            const auto iter1 = m_src_images_buff_map.find(camera_id);
            if (iter1 != m_src_images_buff_map.end())
            {
                m_src_images_buff_map.erase(camera_id);
            }

            const auto iter2 =
                std::find(m_disconnect_camera_id_list.begin(), m_disconnect_camera_id_list.end(), camera_id);
            if (iter2 == m_disconnect_camera_id_list.end())
            {
                LOGI(CameraManager, "camera %s is disconnected!", camera_id.c_str());
                m_disconnect_camera_id_list.push_back(camera_id);
                emit signalCameraStatusChange(camera_id, CameraState::disconnect);
            }
        });

        m_first_init = false;
    }

    auto prev_name_list = std::move(m_camera_id_list);
    m_camera_id_list = std::move(CaptureImage::getCameraInfoList());
    for (const auto &camera_id : m_camera_id_list)
    {
        auto exist = std::find(prev_name_list.begin(), prev_name_list.end(), camera_id);
        if (exist != prev_name_list.end())
        {
            prev_name_list.erase(exist);
            auto &cap = m_capture_map.at(camera_id);
            auto device = cap->getCameraDevice();
            if (device)
            {
                if (device->getCameraStatus())
                {
                    // 指定强制复用或相机已经打开时复用之前创建的camera
                    LOGI(CameraManager, "reuse camera %s!", camera_id.c_str());
                    continue;
                }
            }
            m_capture_map.erase(camera_id);
        }

        // 创建新增的camera
        LOGI(CameraManager, "create camera %s!", camera_id.c_str());

        //如果是已断开的设备，取消断开标记
        auto reconnect_device =
            std::find(m_disconnect_camera_id_list.begin(), m_disconnect_camera_id_list.end(), camera_id);
        if (reconnect_device != m_disconnect_camera_id_list.end())
        {
            LOGI(CameraManager, "camera %s is reconnected!", camera_id.c_str());
            m_disconnect_camera_id_list.erase(reconnect_device);
            emit signalCameraStatusChange(camera_id, CameraState::closed);
        }

        m_capture_map.emplace(std::make_pair(camera_id, std::move(std::make_unique<CaptureImage>(camera_id, this))));
        auto &cap = m_capture_map.at(camera_id);

        connect(cap.get(), &CaptureImage::signalSendRealtimeImage, this,
                [=](const cv::Mat &cv_image) { emit signalSendRealtimeImage(camera_id, cv_image); });
        connect(cap.get(), &CaptureImage::signalSendImage, this,
                [=](const cv::Mat &cv_image, const QString &trigger_char, const QUuid &image_uuid) {
                    if (!cv_image.empty())
                    {
                        if (m_src_images_buff_map.find(camera_id) == m_src_images_buff_map.end())
                            m_src_images_buff_map.emplace(std::make_pair(camera_id, std::move(cv_image)));
                        else
                            m_src_images_buff_map.at(camera_id) = cv_image;
                    }
                    LOGT(CameraManager, "camera %s send image!", camera_id.c_str());
                    LOGT(CameraManager, "The trigger char is %s!", trigger_char.toLatin1().data());
                    emit signalSendImage(camera_id, cv_image, trigger_char, image_uuid);
                });
        connect(cap.get(), &CaptureImage::signalTriggerStart, this, &CameraManager::signalTriggerStart);
        auto ip_info = cap->getCameraDevice()->getCameraIP();
        char ip_address[32] = "";
        sprintf(ip_address, "%d.%d.%d.%d", ip_info.ip >> 24, (ip_info.ip & 0xFF0000) >> 16, (ip_info.ip & 0xFF00) >> 8,
                ip_info.ip & 0xFF);

        connect(cap.get(), &CaptureImage::signalSendCameraStatus, this,
                [&](const std::string &camera_id, bool is_open) {
                    LOGT(CameraManager, "camera %s now is %s!", camera_id.c_str(), is_open ? "opening" : "closed");
                    emit signalCameraStatusChange(camera_id, is_open ? CameraState::opening : CameraState::closed);
                });
    }

    for (auto &camera_id : prev_name_list)  // 清理断开的camera, 标记为断开
    {
        if (std::find(m_disconnect_camera_id_list.begin(), m_disconnect_camera_id_list.end(), camera_id) ==
            m_disconnect_camera_id_list.end())
        {
            LOGI(CameraManager, "camera %s is disconnected!", camera_id.c_str());
            m_disconnect_camera_id_list.push_back(camera_id);
            emit signalCameraStatusChange(camera_id, CameraState::disconnect);
        }
    }

    m_monitor_thread->setCameraIdList(m_camera_id_list);
    LOGI(CameraManager, "init camera finished!");
}

void CameraManager::addVMCamera(const QUuid &view_uuid, const std::string &camera_id)
{
    m_capture_map.emplace(std::make_pair(camera_id, std::move(std::make_unique<CaptureImage>(camera_id, this))));
    auto &cap = m_capture_map.at(camera_id);
    cap->setVMCameraUuid(view_uuid);
    connect(cap.get(), &CaptureImage::signalSendImage, this,
            [=](const cv::Mat &cv_image, const QString &trigger_char, const QUuid &image_uuid) {
                if (!cv_image.empty())
                {
                    if (m_src_images_buff_map.find(camera_id) == m_src_images_buff_map.end())
                        m_src_images_buff_map.emplace(std::make_pair(camera_id, std::move(cv_image)));
                    else
                        m_src_images_buff_map.at(camera_id) = cv_image;
                }
                LOGT(CameraManager, "camera %s send image!", camera_id.c_str());
                LOGT(CameraManager, "The trigger char is %s!", trigger_char.toLatin1().data());
                emit signalSendImage(camera_id, cv_image, trigger_char, image_uuid);
            });
    connect(cap.get(), &CaptureImage::signalTriggerStart, this, &CameraManager::signalTriggerStart);
}

void CameraManager::deleteAllCameras()
{
    for (auto iter = m_capture_map.begin(); iter != m_capture_map.end();)
    {
        const auto &camera_id = iter->first;
        if (camera_id.find("VMCamera") == camera_id.npos)
            iter = m_capture_map.erase(iter);
        else
            iter++;
    }
}

void CameraManager::deleteAllVMCameras()
{
    for (auto iter = m_capture_map.begin(); iter != m_capture_map.end();)
    {
        const auto &camera_id = iter->first;
        if (camera_id.find("VMCamera") != camera_id.npos)
            iter = m_capture_map.erase(iter);
        else
            iter++;
    }
}

void CameraManager::deleteVMCameras(const std::string &camera_id)
{
    auto iter = m_capture_map.find(camera_id);
    if (iter != m_capture_map.end())
        m_capture_map.erase(iter);
}

void CameraManager::setCamerasTriggerMode(int mode)
{
    for (auto &capture : m_capture_map)
    {
        if (capture.second)
            capture.second->setTriggerMode(mode);
    }
}

void CameraManager::resetDisconnectCameraIdList(const std::vector<std::string> &camera_id_list)
{
    m_disconnect_camera_id_list = std::move(camera_id_list);
}

}  // namespace vtk::cameraservice
