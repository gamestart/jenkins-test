/****************************************************************************
 *  @file     camerasetting.h
 *  @brief    设置窗口单个相机设置界面类
 *  @author
 *  @email
 *  @version
 *  @date     2020.12.21
 *  Copyright (c) 2020 SmartMore All rights reserved.
 ****************************************************************************/
#ifndef CAMERASETTING_H
#define CAMERASETTING_H

#include "uiframe/commonuiset/tipwidget/tipwidget.h"

#include "common/vtkcommon.h"
#include <QLineEdit>
#include <QWidget>
#include <opencv2/opencv.hpp>

namespace Ui
{
class CameraSetting;
}

namespace vtk::display
{
struct CameraData
{
    QString camera_name{""};
    bool is_open{false};
};

class CameraSetting : public QWidget
{
    Q_OBJECT

public:
    CameraSetting(QWidget *parent = nullptr, const std::string &camera_name = "", const std::string &camera_id = "");
    ~CameraSetting();
    QString getCameraName();
    QString getCameraId();
    int getTriggerSource();
    int getTriggerActivation();
    void setCameraName(QString);
    bool validateCameraConfig();
    bool validateCameraName();
    bool changeCameraStatus(bool is_open);
    bool changeCameraStatus(vtk::common::CameraState state);
    void disableFocus(int);
    void changeTriggerMode();
    void resetImageSourcesTriggerLevelAndOrder();

signals:
    void signalCameraNameChange(const std::string &name);
    bool signalCheckDuplicate(const std::string &camera_id);

private slots:
    void on_zoom_out_btn_clicked();
    void on_zoom_in_btn_clicked();
    void on_revert_btn_clicked();
    void on_view_img_btn_clicked();

protected:
    void hideEvent(QHideEvent *event) override;
    bool eventFilter(QObject *wcg, QEvent *event) override;

private:
    void init();
    void initWarnings();
    void initWidgets();
    void initConnections();
    void updateDisplay(bool is_open)
    {
        updateDisplay(is_open ? vtk::common::CameraState::opening : vtk::common::CameraState::closed);
    }
    void updateDisplay(vtk::common::CameraState state);
    void updateGain();
    void updateExposure();
    void updateGamma();
    void updateAvailableTriggerSource();
    void updateAvailableTriggerActivation();
    void updateTriggerModeCombox();
    int getCurrentTriggerSourceValueFromConfigdata();
    int getCurrentTriggerActivationValueFromConfigdata();
    void updateIP();
    void updateSubnetmask();
    void openCamera();
    void closeCamera();
    void stopCapture();
    bool isOccupiedOfCurrentIP(const QString &ip);
    bool validateCameraIp();
    bool validateCameraSubnetMask();
    bool validateCameraExposure();
    bool validateCameraGain();
    bool validateCameraGamma();
    void interceptImage(const cv::Mat &cv_image);

private:
    Ui::CameraSetting *ui;
    std::string m_camera_name;
    std::string m_camera_id;

    bool m_is_open = false;
    TipWidget *m_tip_widget{nullptr};
};
}  // namespace vtk::display
Q_DECLARE_METATYPE(vtk::display::CameraData);
#endif  // CAMERASETTING_H
