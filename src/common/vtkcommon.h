/****************************************************************************
 *  @file     vtkcommon.h
 *  @brief    vtk通用函数实现文件
 *  @author   junjie.zeng
 *  @email    junjie.zeng@smartmore.com
 *  @version
 *  @date     2020.11.13
 *  Copyright (c) 2020 SmartMore All rights reserved.
 ****************************************************************************/

#pragma once
#include "applogger.h"
#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QImage>
#include <QSettings>
#include <QUuid>
#include <memory>
#include <opencv2/opencv.hpp>

namespace vtk::common
{

#define DISALLOW_COPY_AND_ASSIGN(T)                                                                                    \
    T(const T &) = delete;                                                                                             \
    T &operator=(const T &) = delete;                                                                                  \
    T(const T &&) = delete;                                                                                            \
    T &operator=(const T &&) = delete;

enum class CameraState
{
    disconnect = 0,  //相机断开状态
    closed,          //相机关闭状态
    opening          //相机打开状态
};

enum class AlgoType
{
    noType = 0,
    segmentationType,    //分割类型
    detectionType,       //检测类型
    classificationType,  //分类类型
    ocrType              // OCR类型
};

enum class ResultStatusType
{
    notAvailable = 0,
    ok,
    ng
};

struct AlgoResult
{
    size_t elapsed_time{0};
    cv::Mat image;
    cv::Mat ai_image;
    std::vector<std::string> lab_names;
    ResultStatusType result_status{ResultStatusType::notAvailable};

    bool isOk() const
    {
        return (result_status != ResultStatusType::ng);
    }
};

struct IntegratedAlgoResult
{
    size_t elapsed_time{0};
    cv::Mat image;
    std::vector<cv::Mat> ai_images;
    std::vector<std::string> lab_names;
    ResultStatusType result_status{ResultStatusType::notAvailable};
};

class Message : public QObject
{
    Q_OBJECT
public:
    enum MessageType
    {
        info = 0,
        warning,
        error
    };

    static Message &getInstance()
    {
        static Message s_message;
        return s_message;
    }
    static void postInfoMessage(const QString &msg)
    {
        emit getInstance().signalPostMessage(msg, MessageType::info);
    }

    static void postWarningMessage(const QString &msg)
    {
        emit getInstance().signalPostMessage(msg, MessageType::warning);
    }

    static void postErrorMessage(const QString &msg)
    {
        emit getInstance().signalPostMessage(msg, MessageType::error);
    }

signals:
    void signalPostMessage(const QString &msg, vtk::common::Message::MessageType type);

private:
    Message()
    {
        qRegisterMetaType<vtk::common::Message::MessageType>("vtk::common::Message::MessageType");
    }
};

class ImageListForwarder : public QObject
{
    Q_OBJECT
public:
    struct ImageInfo
    {
        QString image_path;
        QUuid image_uuid;
    };
    static ImageListForwarder &getInstance()
    {
        static ImageListForwarder s_forwarder;
        return s_forwarder;
    }

signals:
    void signalPostImageListInfo(const QUuid &view_uuid, const QList<ImageInfo> &image_info_list);

private:
    ImageListForwarder() = default;
};

class CameraStatusGetter : public QObject
{
    Q_OBJECT
public:
    static CameraStatusGetter &getInstance()
    {
        static CameraStatusGetter s_getter;
        return s_getter;
    }

signals:
    vtk::common::CameraState signalGetCameraStatus(const QString &camera_name);

private:
    CameraStatusGetter() = default;
};

using AlgoHandle = int;

bool isIpReachable(const QString &ip, int wait_ms);
/**
 * @brief isSameCameraId 判断视窗所选相机的camera_id 与输入的camera_id是否相同
 * @param input_camera_id
 * @return
 */
bool isSameCameraId(const QUuid &view_uuid, const std::string &input_camera_id);

/**
 * @brief isCameraInUse 判断相机是否被使用
 * @param input_camera_id
 * @return
 */
bool isCameraInUse(const std::string &input_camera_id);

/**
 * @brief getCameraIdByViewId 通过view_uuid找到camera_id
 * @param view_uuid
 * @return
 */
std::string getCameraIdByViewId(const QUuid &view_uuid);

/**
 * @brief matToQImage  cv::mat格式图片转QImage格式图片
 * @param mat          输入cv::mat格式图片
 * @return             输出QImage格式图片
 */
QImage matToQImage(const cv::Mat &mat);

/**
 * @brief utfToGbk     utf8编码转gbk编码
 * @param              输入utf8编码的QString
 * @return             输出gbk编码的std:string
 */
std::string utf8ToGbk(const QString &);

/**
 * @brief containsChineseSpecial     检查字符串是否含有中文特殊字符
 * @param              输入QString
 * @return             输出bool
 */
bool containsChineseSpecial(QString);

/**
 * @brief containsSpecial     检查字符串是否含有特殊字符（不包括中文字符）
 * @param              输入std::string
 * @return             输出bool
 */
bool containsSpecial(std::string);

/**
 * @brief containsAllSpecial     检查字符串是否含有特殊字符（包括中文字符）
 * @param              输入QString
 * @return             输出bool
 */
bool containsAllSpecial(QString);

}  // namespace vtk::common
Q_DECLARE_METATYPE(vtk::common::CameraState);
