/****************************************************************************
 *  @file     cameradevice.h
 *  @brief    相机设备
 *  @author   junjie.zeng
 *  @email    junjie.zeng@smartmore.com
 *  @version
 *  @date     2020.10.15
 *  Copyright (c) 2020 SmartMore All rights reserved.
 ****************************************************************************/

#ifndef CAMERADEVICE_H
#define CAMERADEVICE_H

#include "device.h"
#include <QMutex>
#include <QObject>
#include <memory>

namespace vtk::cameraservice
{

class CameraDevice : public QObject
{
    Q_OBJECT
public:
    using DeviceInfo = smartmore::DeviceInfo;
    using DeviceList = smartmore::DeviceList;
    using IPInfo = smartmore::IPInfo;
    using TriggerSource = smartmore::TriggerSource;
    using TriggerActivation = smartmore::TriggerActivation;
    using AcquisitionMode = smartmore::AcquisitionMode;

    CameraDevice(const std::string &camera_id);
    virtual ~CameraDevice() = default;

    static std::vector<std::string> getCameraInfoList();

    static int addVirtualCamera(const std::string &camera_id);
    static int removeVirtualCamera(const std::string &camera_id);

    /**
     * @brief grabImage  抓取图片
     * @param obj        camera对象指针
     * @param image      抓取到的图片
     */
    static void grabImage(void *obj, const cv::Mat &image);

    bool isConnected();

    int32_t initCamera();

    int32_t openCamera();
    int32_t closeCamera();

    int32_t startGrabbing();
    int32_t stopGrabbing();

    void triggerOne();

    bool getTriggerModeStatus();
    int32_t setTriggerModeStatus(bool open);

    IPInfo getCameraIP();
    int32_t setCameraIP(const IPInfo &ip_info);

    float getExposureTime();
    int32_t setExposureTime(float exposure_time);

    float getGain();
    int32_t setGain(float gain);

    float getGamma();
    int32_t setGamma(float gamma);

    int32_t setTriggerSource(TriggerSource trigger_source);
    TriggerSource getTriggerSource();

    int setTriggerActivation(TriggerActivation trigger_activation);
    TriggerActivation getTriggerActivation();

    AcquisitionMode getAcquisitionMode();
    int32_t setAcquisitionMode(AcquisitionMode acq_mode);

    int getAvailableTriggerSource(std::vector<int> &values);
    int getAvailableTriggerActivation(std::vector<int> &values);

    /**
     * @brief getCameraStatus 获取相机状态
     * @return
     */
    inline bool getCameraStatus() const
    {
        return m_is_open;
    }

    const smartmore::DeviceInfo &getDeviceInfo()
    {
        return m_device_info;
    }

    void setVMCameraUuid(const QUuid &view_uuid);
    const QUuid getVMCameraUuid() const;

signals:
    void signalSendImage(const cv::Mat &cv_image, const QUuid &image_uuid);
    void signalSendCameraIp(const std::string &camera_id, const std::string &camera_ip);
    void signalSendCameraStatus(const std::string &camera_id, bool is_open);

protected:
    std::string m_camera_id;
    bool m_is_open{false};
    bool m_is_grabbing{false};

private:
    std::shared_ptr<smartmore::CameraBase> m_device{nullptr};
    DeviceInfo m_device_info;
    static QMutex m_mutex;

    static bool findDeviceInfo(const std::string &camera_name, DeviceInfo &device_info);
    static smartmore::DeviceList m_cached_device_list;
};

}  // namespace vtk::cameraservice

#endif  // CAMERADEVICE_H
