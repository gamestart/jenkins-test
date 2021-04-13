/****************************************************************************
 *  @file     virtualcamera.h
 *  @brief    虚拟相机设备类
 *  @author   junjie.zeng
 *  @email    junjie.zeng@smartmore.com
 *  @version
 *  @date     2021.01.18
 *  Copyright (c) 2021 SmartMore All rights reserved.
 ****************************************************************************/
#ifndef VIRTUALCAMERA_H
#define VIRTUALCAMERA_H

#include "common/vtkcommon.h"
#include "device.h"
#include <QObject>
#include <QTextCodec>
#include <QUuid>
#include <string>

namespace vtk::cameraservice
{
using namespace vtk::common;

class VirtualCamera : public QObject, public smartmore::CameraBase
{
    Q_OBJECT

public:
    struct ImagesData
    {
        size_t current_index{0};
        QList<ImageListForwarder::ImageInfo> image_list;
    };

    VirtualCamera(QObject *parent = nullptr);
    ~VirtualCamera();

    int initCamera() override;
    int uninitCamera() override;
    int openCamera() override;
    int closeCamera() override;
    int registerImageCallBack(smartmore::OnFrameReady callback, void *cb_param) override;
    bool isConnected() override;
    int startGrabbing() override;
    int stopGrabbing() override;
    void triggerOne() override;
    int setCameraIP(const smartmore::IPInfo &ip_info) override;
    smartmore::IPInfo getCameraIP() override;
    int getIntValue(smartmore::DeviceIntParam param) override;
    int setIntValue(smartmore::DeviceIntParam param, int value) override;
    float getFloatValue(smartmore::DeviceFloatParam param) override;
    int setFloatValue(smartmore::DeviceFloatParam param, float value) override;
    bool getBoolValue(smartmore::DeviceBoolParam param) override;
    int setBoolValue(smartmore::DeviceBoolParam param, bool value) override;
    int getAvailableEnumValues(smartmore::DeviceEnumParam param, std::vector<int> &values) override;
    int getEnumValue(smartmore::DeviceEnumParam param) override;
    int setEnumValue(smartmore::DeviceEnumParam param, int value) override;
    std::string getStrValue(smartmore::DeviceStrParam param) override;
    int setStrValue(smartmore::DeviceStrParam param, const std::string &value) override;
    void setVMCameraUuid(const QUuid &view_uuid);
    const QUuid &getVMCameraUuid() const;

signals:
    void signalSendImageInfo(const QUuid &image_uuid, const cv::Mat &image);

private:
    smartmore::OnFrameReady m_callback;
    void *m_cb_param;
    ImagesData m_data;
    QUuid m_uuid;
};
}  // namespace vtk::cameraservice
#endif  // VIRTUALCAMERA_H
