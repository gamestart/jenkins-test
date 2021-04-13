/****************************************************************************
 *  @file     virtualcamera.cpp
 *  @brief    虚拟相机设备类
 *  @author   junjie.zeng
 *  @email    junjie.zeng@smartmore.com
 *  @version
 *  @date     2021.01.18
 *  Copyright (c) 2021 SmartMore All rights reserved.
 ****************************************************************************/
#include "virtualcamera.h"

namespace vtk::cameraservice
{
using namespace vtk::common;

VirtualCamera::VirtualCamera(QObject *parent) : QObject(parent)
{
    connect(&ImageListForwarder::getInstance(), &ImageListForwarder::signalPostImageListInfo, this,
            [&](const QUuid &view_uuid, const QList<ImageListForwarder::ImageInfo> &image_list) {
                if (view_uuid == m_uuid)
                    m_data.image_list.append(image_list);
            });
}

VirtualCamera::~VirtualCamera() {}

int VirtualCamera::initCamera()
{
    return 0;
}

int VirtualCamera::uninitCamera()
{
    return 0;
}

int VirtualCamera::openCamera()
{
    return 0;
}

int VirtualCamera::closeCamera()
{
    return 0;
}

int VirtualCamera::registerImageCallBack(smartmore::OnFrameReady callback, void *cb_param)
{
    m_callback = callback;
    m_cb_param = cb_param;
    return 0;
}

bool VirtualCamera::isConnected()
{
    return true;
}

int VirtualCamera::startGrabbing()
{
    return 0;
}

int VirtualCamera::stopGrabbing()
{
    return 0;
}

void VirtualCamera::triggerOne()
{
    cv::Mat input_image;
    QUuid image_uuid;
    const auto size = m_data.image_list.size();
    if (size > 0 && m_data.current_index < size)
    {
        LOGI(VirtualCamera, "Current Image index is %d.", m_data.current_index);
        image_uuid = m_data.image_list.at(m_data.current_index).image_uuid;
        auto file_path = m_data.image_list.at(m_data.current_index).image_path;
#ifdef Q_OS_WIN
        auto gbk_code = QTextCodec::codecForName("GBK");
        auto path = std::string(gbk_code->fromUnicode(file_path).data());
#else
        auto path = file_path.toStdString();
#endif
        input_image = cv::imread(path);
        m_data.current_index++;
    }
    if (input_image.empty())
        LOGW(VirtualCamera, "Virtual camera input image is empty!");

    emit signalSendImageInfo(image_uuid, input_image);
}

int VirtualCamera::setCameraIP(const smartmore::IPInfo &ip_info)
{
    (void)ip_info;
    return 0;
}

smartmore::IPInfo VirtualCamera::getCameraIP()
{
    return smartmore::IPInfo{};
}

int VirtualCamera::getIntValue(smartmore::DeviceIntParam param)
{
    (void)param;
    return 0;
}

int VirtualCamera::setIntValue(smartmore::DeviceIntParam param, int value)
{
    (void)param;
    (void)value;
    return 0;
}

float VirtualCamera::getFloatValue(smartmore::DeviceFloatParam param)
{
    (void)param;
    return 0.0f;
}

int VirtualCamera::setFloatValue(smartmore::DeviceFloatParam param, float value)
{
    (void)param;
    (void)value;
    return 0;
}

bool VirtualCamera::getBoolValue(smartmore::DeviceBoolParam param)
{
    (void)param;
    return true;
}

int VirtualCamera::setBoolValue(smartmore::DeviceBoolParam param, bool value)
{
    (void)param;
    (void)value;
    return 0;
}

int VirtualCamera::getAvailableEnumValues(smartmore::DeviceEnumParam param, std::vector<int> &values)
{
    (void)param;
    (void)values;
    return 0;
}

int VirtualCamera::getEnumValue(smartmore::DeviceEnumParam param)
{
    (void)param;
    return 0;
}

int VirtualCamera::setEnumValue(smartmore::DeviceEnumParam param, int value)
{
    (void)param;
    (void)value;
    return 0;
}

std::string VirtualCamera::getStrValue(smartmore::DeviceStrParam param)
{
    (void)param;
    return "";
}

int VirtualCamera::setStrValue(smartmore::DeviceStrParam param, const std::string &value)
{
    (void)param;
    (void)value;
    return 0;
}

void VirtualCamera::setVMCameraUuid(const QUuid &view_uuid)
{
    m_uuid = view_uuid;
}

const QUuid &VirtualCamera::getVMCameraUuid() const
{
    return m_uuid;
}

}  // namespace vtk::cameraservice
