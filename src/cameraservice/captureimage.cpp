/****************************************************************************
 *  @file     captureimage.cpp
 *  @brief    相机采集图像类
 *  @author   junjie.zeng
 *  @email    junjie.zeng@smartmore.com
 *  @version
 *  @date     2020.11.13
 *  Copyright (c) 2020 SmartMore All rights reserved.
 ****************************************************************************/

#include "captureimage.h"
#include "common/vtkcommon.h"
#include "virtualcamera.h"

namespace vtk::cameraservice
{

using Message = vtk::common::Message;

CaptureImage::CaptureImage(const std::string &camera_id, QObject *parent) : QObject(parent)
{
    init(camera_id);
}

CaptureImage::~CaptureImage()
{
    stopCapture();
    closeCamera();
}

void CaptureImage::startCapture()
{
    if (m_camera && m_camera->getCameraStatus())
    {
        if (m_camera->startGrabbing())
        {
            Message::postWarningMessage(tr("Failed to grabbing!"));
            return;
        }
    }
    else
    {
        Message::postWarningMessage(tr("Please check camera connection condition and open the camera"));
        return;
    }
    m_enable_realtime_capture = true;
    m_camera->triggerOne();
}

bool CaptureImage::startCapture(const QList<QString> &trigger_char_list)
{
    if (m_camera && m_camera->getCameraStatus())
    {
        if (m_camera->startGrabbing())
        {
            Message::postWarningMessage(tr("Failed to grabbing!"));
            return false;
        }
    }
    else
    {
        Message::postWarningMessage(tr("Camera is not exist or not open!"));
        return false;
    }

    m_trigger_char_list.append(trigger_char_list);
    emit signalTriggerStart(m_trigger_char_list.first());
    if (m_trigger_mode == TrigerMode::softTrigger)
        m_camera->triggerOne();
    return true;
}

void CaptureImage::stopCapture()
{
    m_enable_realtime_capture = false;
    if (m_camera)
        m_camera->stopGrabbing();
}

void CaptureImage::setTriggerMode(int mode)
{
    m_trigger_mode = static_cast<TrigerMode>(mode);
}

std::vector<std::string> CaptureImage::getCameraInfoList()
{
    return vtk::cameraservice::CameraDevice::getCameraInfoList();
}

void CaptureImage::setVMCameraUuid(const QUuid &view_uuid)
{
    if (m_camera)
        m_camera->setVMCameraUuid(view_uuid);
}

void CaptureImage::init(const std::string &camera_id)
{
    initCameraDevice(camera_id);
    connect(m_camera.get(), &CameraDevice::signalSendCameraIp, this, &CaptureImage::signalSendCameraIp);
    connect(m_camera.get(), &CameraDevice::signalSendCameraStatus, this, &CaptureImage::signalSendCameraStatus);
}

void CaptureImage::initCameraDevice(const std::string &camera_id)
{
    m_camera = std::make_shared<vtk::cameraservice::CameraDevice>(camera_id);
    connect(
        m_camera.get(), &CameraDevice::signalSendImage, this,
        [&](const cv::Mat &cv_image, const QUuid &image_uuid) {
            if (m_trigger_mode == TrigerMode::hardTrigger)  //硬触发处理
            {
                if (m_trigger_char_list.isEmpty())
                {
                    emit signalSendRealtimeImage(cv_image);
                }
                else
                {
                    emit signalSendImage(cv_image, m_trigger_char_list.takeFirst(), image_uuid);
                }
            }
            else
            {
                if (m_trigger_char_list.isEmpty())
                {
                    emit signalSendRealtimeImage(cv_image);
                    if (m_enable_realtime_capture)
                        m_camera->triggerOne();
                }
                else
                {
                    emit signalSendImage(cv_image, m_trigger_char_list.takeFirst(), image_uuid);
                }
                if (!m_trigger_char_list.isEmpty())
                {
                    emit signalTriggerStart(m_trigger_char_list.first());
                    m_camera->triggerOne();
                }
            }
        },
        Qt::UniqueConnection);
}

void CaptureImage::closeCamera()
{
    if (m_camera)
        m_camera->closeCamera();
}

}  // namespace vtk::cameraservice
