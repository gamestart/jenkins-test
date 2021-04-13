/****************************************************************************
 *  @file     cameramanager.cpp
 *  @brief    相机设备管理员
 *  @author   junjie.zeng
 *  @email    junjie.zeng@smartmore.com
 *  @version
 *  @date     2020.10.15
 *  Copyright (c) 2020 SmartMore All rights reserved.
 ****************************************************************************/

#ifndef CAMERAMANAGER_H
#define CAMERAMANAGER_H

#include <QMutex>
#include <QObject>
#include <QVariant>
#include <memory>
#include <opencv2/opencv.hpp>
#include <unordered_map>

#include "captureimage.h"
#include "common/vtkcommon.h"
#include "monitorthread.h"
#include <QList>

namespace vtk::cameraservice
{

class CaptureImage;

class CameraManager : public QObject
{
    Q_OBJECT
public:
    static CameraManager &getInstance();
    ~CameraManager();

    void init();
    void stopCamerasCapture();
    void startCameraCapture(const std::string &camera_id);
    bool startCameraCapture(const std::string &camera_id, const QList<QString> &trigger_char_list);
    void stopCameraCapture(const std::string &camera_id);
    void clearCameras();

    inline const std::vector<std::string> getCameraIdList() const
    {
        return m_camera_id_list;
    }

    inline const std::vector<std::string> getDisconnectCameraIdList() const
    {
        return m_disconnect_camera_id_list;
    }

    inline std::shared_ptr<CameraDevice> getCameraDevice(const std::string &camera_id)
    {
        if (m_capture_map.find(camera_id) != m_capture_map.end() && m_capture_map.at(camera_id))
        {
            return m_capture_map.at(camera_id)->getCameraDevice();
        }
        return nullptr;
    }

    inline const std::unordered_map<std::string, cv::Mat> &getCaptureImages() const
    {
        return m_src_images_buff_map;
    }

    template <typename T> const std::vector<QVariant> infosToVariant(size_t counts, const T &list)
    {
        std::vector<QVariant> infos;
        for (size_t i = 0; i < counts; i++)
            infos.emplace_back(QVariant::fromValue(list[i]));
        return infos;
    };

    void deleteDisconnectedCamera(const std::string &camera_id);
    void addVMCamera(const QUuid &view_uuid, const std::string &camera_id);
    void deleteAllCameras();
    void deleteAllVMCameras();
    void deleteVMCameras(const std::string &camera_id);
    void setCamerasTriggerMode(int mode);
    void resetDisconnectCameraIdList(const std::vector<std::string> &camera_id_list);

signals:
    void signalSendRealtimeImage(const std::string &camera_id, const cv::Mat &cv_image);
    void signalSendImage(const std::string &camera_id, const cv::Mat &cv_iamge, const QString &trigger_char = "",
                         const QUuid &image_uuid = "");
    void signalTriggerStart(const QString &trigger_char);
    void signalCameraStatusChange(const std::string &camera_id, vtk::common::CameraState state);

private:
    CameraManager() = default;
    DISALLOW_COPY_AND_ASSIGN(CameraManager)

private:
    std::unordered_map<std::string, std::unique_ptr<CaptureImage>> m_capture_map;  //<camera_id, capture>
    std::unordered_map<std::string, cv::Mat> m_src_images_buff_map;                //<camera_id, src_images_buff>
    std::vector<std::string> m_camera_id_list;
    std::vector<std::string> m_disconnect_camera_id_list;
    std::unique_ptr<MonitorThread> m_monitor_thread{nullptr};
    bool m_first_init{true};
};

}  // namespace vtk::cameraservice
#endif  // CAMERAMANAGER_H
