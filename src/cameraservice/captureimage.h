/****************************************************************************
 *  @file     captureimage.h
 *  @brief    相机采集图像类
 *  @author   junjie.zeng
 *  @email    junjie.zeng@smartmore.com
 *  @version
 *  @date     2020.11.13
 *  Copyright (c) 2020 SmartMore All rights reserved.
 ****************************************************************************/

#pragma once

#include "cameradevice.h"
#include <QList>
#include <QObject>
#include <opencv2/opencv.hpp>

namespace vtk::cameraservice
{

class CaptureImage : public QObject
{
    Q_OBJECT
public:
    enum class TrigerMode : int
    {
        softTrigger = 0,
        hardTrigger
    };
    CaptureImage(const std::string &camera_id = std::string(), QObject *parent = nullptr);
    ~CaptureImage();

    void startCapture();
    bool startCapture(const QList<QString> &trigger_char_list);
    void stopCapture();
    void setTriggerMode(int mode);

    inline std::shared_ptr<vtk::cameraservice::CameraDevice> getCameraDevice() const
    {
        return m_camera;
    }
    static std::vector<std::string> getCameraInfoList();

    void setVMCameraUuid(const QUuid &view_uuid);

signals:
    void signalSendRealtimeImage(const cv::Mat &cv_image);
    void signalSendImage(const cv::Mat &cv_image, const QString &trigger_char, const QUuid &image_uuid);
    void signalTriggerStart(const QString &trigger_char);
    void signalSendCameraIp(const std::string &camera_id, const std::string &camera_ip);
    void signalSendCameraStatus(const std::string &camera_id, bool is_open);

private:
    void init(const std::string &camera_id);
    void initCameraDevice(const std::string &camera_id);
    void closeCamera();

private:
    std::shared_ptr<vtk::cameraservice::CameraDevice> m_camera{nullptr};
    QList<QString> m_trigger_char_list;
    bool m_enable_realtime_capture{false};
    TrigerMode m_trigger_mode{TrigerMode::softTrigger};
};

}  // namespace vtk::cameraservice
