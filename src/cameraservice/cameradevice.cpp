/****************************************************************************
 *  @file     cameradevice.cpp
 *  @brief    相机设备
 *  @author   junjie.zeng
 *  @email    junjie.zeng@smartmore.com
 *  @version
 *  @date     2020.10.15
 *  Copyright (c) 2020 SmartMore All rights reserved.
 ****************************************************************************/

#include "cameradevice.h"
#include "common/applogger.h"
#include "virtualcamera.h"

#ifdef HIK
#include "hikcamera.h"
#endif
#ifdef BASLER
#include "baslercamera.h"
#endif
#ifdef DALSA
#include "dalsagige.h"
#endif

namespace vtk::cameraservice
{

smartmore::DeviceList CameraDevice::m_cached_device_list;
QMutex CameraDevice::m_mutex;

void CameraDevice::grabImage(void *obj, const cv::Mat &image)
{
    auto camera = static_cast<CameraDevice *>(obj);
    if (camera)
        emit camera->signalSendImage(image, "");
}

CameraDevice::CameraDevice(const std::string &camera_id) : m_camera_id(camera_id)
{
    initCamera();
}

int32_t CameraDevice::initCamera()
{
    if (m_device)
    {
        m_device->uninitCamera();
    }
    if (findDeviceInfo(m_camera_id, m_device_info))
    {
        int32_t ret{0};
        switch (m_device_info.device_type)
        {
#ifdef HIK
            case smartmore::DeviceType::Hik:
                m_device.reset(new smartmore::HikCamera(m_device_info, spdlog::level::trace, m_camera_id));
                break;
#endif
#ifdef BASLER
            case smartmore::DeviceType::Basler:
                m_device.reset(new smartmore::BaslerCamera(m_device_info, spdlog::level::trace, m_camera_id));
                break;
#endif
#ifdef DALSA
            case smartmore::DeviceType::Dalsa:
                m_device.reset(new smartmore::DalsaGigeCamera(m_device_info, spdlog::level::trace, m_camera_id));
                break;
#endif
            default:
                return -1;
        }
        ret = m_device->initCamera();
        m_device->registerImageCallBack(CameraDevice::grabImage, this);
        auto ip_info = m_device->getCameraIP();
        char ip_address[32] = "";
        sprintf(ip_address, "%d.%d.%d.%d", ip_info.ip >> 24, (ip_info.ip & 0xFF0000) >> 16, (ip_info.ip & 0xFF00) >> 8,
                ip_info.ip & 0xFF);
        emit signalSendCameraIp(m_camera_id, std::string(ip_address));
        emit signalSendCameraStatus(m_camera_id, false);
        return ret;
    }
    else if (m_camera_id.find("VMCamera") != m_camera_id.npos)
    {
        m_device_info.device_type = smartmore::DeviceType::Emulator;
        m_device.reset(new VirtualCamera());
        connect(dynamic_cast<VirtualCamera *>(m_device.get()), &VirtualCamera::signalSendImageInfo, this,
                [&](const QUuid &image_uuid, const cv::Mat &image) { emit signalSendImage(image, image_uuid); });
        m_is_open = true;
        return 0;
    }
    return -1;
}

std::vector<std::string> CameraDevice::getCameraInfoList()
{
    smartmore::DeviceList device_list;
#ifdef HIK
    smartmore::HikCamera::enumCamera(device_list);
#endif
#ifdef BASLER
    smartmore::BaslerCamera::enumCamera(device_list);
#endif
#ifdef DALSA
    smartmore::DalsaGigeCamera::enumCamera(device_list);
#endif

    {
        QMutexLocker lock(&m_mutex);
        m_cached_device_list = std::move(device_list);
    }

    std::vector<std::string> id_list;
    for (auto &info : m_cached_device_list)
    {
        auto camera_id = info.model_name + info.serial_number;
        id_list.push_back(camera_id);
    }
    return id_list;
}

bool CameraDevice::findDeviceInfo(const std::string &camera_id, smartmore::DeviceInfo &device_info)
{
    QMutexLocker lock(&m_mutex);
    for (auto &info : m_cached_device_list)
    {
        auto cur_camera_id = info.model_name + info.serial_number;
        if (camera_id == cur_camera_id)
        {
            device_info = info;
            return true;
        }
    }
    LOGW(CameraDevice, "can't find device \'%s\'", camera_id.c_str());
    return false;
}

bool CameraDevice::isConnected()
{
    return m_device ? m_device->isConnected() : false;
}

int32_t CameraDevice::openCamera()
{
    if (m_is_open)
        return 0;
    int32_t ret{-1};
    if (m_device)
    {
        ret = m_device->openCamera();  //打开相机
        if (ret)
            goto open_failed;
        m_is_open = true;
        if (m_device_info.device_type == smartmore::DeviceType::Dalsa)
        {
            // dalsa 必须用单次采集模式
            setAcquisitionMode(AcquisitionMode::SingleMode);  //单次采集模式
        }
        else
        {
            setAcquisitionMode(AcquisitionMode::ContinuousMode);  //循环采集模式
        }
        setTriggerModeStatus(true);  //触发模式打开
        emit signalSendCameraStatus(m_camera_id, true);
    }

open_failed:
    return ret;
}

int32_t CameraDevice::closeCamera()
{
    int32_t ret{0};
    if (!m_is_open)
        return ret;
    stopGrabbing();
    if (m_device)
        ret = m_device->closeCamera();
    m_is_open = false;
    emit signalSendCameraStatus(m_camera_id, false);
    return ret;
}

int32_t CameraDevice::startGrabbing()
{
    int32_t ret{0};
    if (m_is_grabbing)
        return ret;
    if (m_device && m_is_open && (ret = m_device->startGrabbing()) == 0)
    {
        m_is_grabbing = true;
    }
    return ret;
}

int32_t CameraDevice::stopGrabbing()
{
    int32_t ret{0};
    if (!m_is_grabbing)
        return ret;
    m_is_grabbing = false;
    return m_device && m_is_open ? m_device->stopGrabbing() : ret;
}

void CameraDevice::triggerOne()
{
    if (m_device && m_is_open)
    {
        LOGW(CameraDevice, "%s trigger start", m_camera_id.c_str());
        m_device->triggerOne();
    }
}

bool CameraDevice::getTriggerModeStatus()
{
    return m_device && m_is_open ? (m_device->getTriggerModeStatus() == 0 ? false : true) : true;
}

int32_t CameraDevice::setTriggerModeStatus(bool open)
{
    return m_device && m_is_open ? m_device->setTriggerModeStatus(open ? 1 : 0) : 0;
}

CameraDevice::IPInfo CameraDevice::getCameraIP()
{
    if (m_device)
    {
        return m_device->getCameraIP();
    }
    return IPInfo();
}

int32_t CameraDevice::setCameraIP(const IPInfo &ip_info)
{
    int32_t ret{-1};
    if (m_device)
    {
        smartmore::IPInfo hik_ip_info{ip_info.ip, ip_info.subnetmask, ip_info.gateway};
        ret = m_device->setCameraIP(hik_ip_info);
    }
    return ret;
}

float CameraDevice::getExposureTime()
{
    return m_device && m_is_open ? m_device->getExposureTime() : 0.0f;
}

int32_t CameraDevice::setExposureTime(float exposure_time)
{
    return m_device && m_is_open ? m_device->setExposureTime(exposure_time) : 0;
}

float CameraDevice::getGain()
{
    return m_device && m_is_open ? m_device->getGain() : 0.0f;
}

int32_t CameraDevice::setGain(float gain)
{
    return m_device && m_is_open ? m_device->setGain(gain) : 0;
}

float CameraDevice::getGamma()
{
    return m_device ? m_device->getGamma() : 0.0f;
}

int32_t CameraDevice::setGamma(float gamma)
{
    int32_t ret{0};
    return m_device ? m_device->setGain(gamma) : ret;
}

CameraDevice::TriggerSource CameraDevice::getTriggerSource()
{
    return m_device && m_is_open ? static_cast<TriggerSource>(m_device->getTriggerSource()) : TriggerSource::SoftType;
}

int CameraDevice::setTriggerActivation(TriggerActivation trigger_activation)
{
    return m_device && m_is_open ? m_device->setTriggerActivation(static_cast<int>(trigger_activation)) : 0;
}

CameraDevice::TriggerActivation CameraDevice::getTriggerActivation()
{
    return m_device && m_is_open ? static_cast<TriggerActivation>(m_device->getTriggerActivation()) :
                                   TriggerActivation::UnknownTriggerActivation;
}

int32_t CameraDevice::setTriggerSource(TriggerSource trigger_source)
{
    return m_device && m_is_open ? m_device->setTriggerSource(static_cast<int>(trigger_source)) : 0;
}

CameraDevice::AcquisitionMode CameraDevice::getAcquisitionMode()
{
    return m_device && m_is_open ? static_cast<AcquisitionMode>(m_device->getAcquisitionMode()) :
                                   AcquisitionMode::ContinuousMode;
}

int32_t CameraDevice::setAcquisitionMode(AcquisitionMode acq_mode)
{
    return m_device && m_is_open ? m_device->setAcquisitionMode(acq_mode) : 0;
}

int CameraDevice::getAvailableTriggerSource(std::vector<int> &values)
{
    return m_device && m_is_open ? m_device->getAvailableTriggerSource(values) : 0;
}

int CameraDevice::getAvailableTriggerActivation(std::vector<int> &values)
{
    return m_device && m_is_open ? m_device->getAvailableTriggerActivation(values) : 0;
}

void CameraDevice::setVMCameraUuid(const QUuid &view_uuid)
{
    auto vmcmaera = std::dynamic_pointer_cast<VirtualCamera>(m_device);
    if (vmcmaera)
        vmcmaera->setVMCameraUuid(view_uuid);
}

const QUuid CameraDevice::getVMCameraUuid() const
{
    auto vmcmaera = std::dynamic_pointer_cast<vtk::cameraservice::VirtualCamera>(m_device);
    if (vmcmaera)
        return vmcmaera->getVMCameraUuid();
    return QUuid();
}

}  // namespace vtk::cameraservice
