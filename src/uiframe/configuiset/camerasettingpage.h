/****************************************************************************
 *  @file     camerasettingpage.h
 *  @brief    设置窗口相机设置界面类
 *  @author
 *  @email
 *  @version
 *  @date     2020.12.21
 *  Copyright (c) 2020 SmartMore All rights reserved.
 ****************************************************************************/
#ifndef CAMERASETTINGPAGE_H
#define CAMERASETTINGPAGE_H

#include "camerasetting.h"
#include "common/vtkcommon.h"

#include <QLabel>
#include <QListWidget>
#include <QToolButton>
#include <QWidget>

namespace Ui
{
class CameraSettingPage;
}

namespace vtk::display
{
class CameraItem : public QWidget
{
    Q_OBJECT
public:
    struct ItemData
    {
        std::string camera_name{""};
        std::string camera_id{""};
    };
    CameraItem(vtk::common::CameraState state = vtk::common::CameraState::closed,
               const QString &camera_name = QString(""), QWidget *parent = nullptr);
    ~CameraItem() = default;

    inline vtk::common::CameraState getCameraStatus()
    {
        return m_state;
    }

    void setCameraStatus(vtk::common::CameraState state);

    const QString getText() const;
    void setText(const QString &txt);

    enum class Action
    {
        DeleteCamera
    };

signals:
    bool signalSendCameraStatus(bool is_open);
    int signalMenuAction(Action action);

private:
    vtk::common::CameraState m_state;
    QLabel *m_camera_status_lab;
    QLabel *m_name_lab;
    QAction *m_delete_menu;
    QToolButton *m_switch_btn;
};

class CameraSettingPage : public QWidget
{
    Q_OBJECT

public:
    explicit CameraSettingPage(QWidget *parent = nullptr);
    ~CameraSettingPage();
    void initCameraListWidget();
    bool validateCameraSettings();
    void saveCameraConfigData();
    void cancelCameraConfig();
    void disableFocus();

private slots:
    bool slotCheckDuplicate(const std::string &);
    std::vector<CameraData> slotAddNewCamera();
    void slotRefreshCameraList();
    vtk::common::CameraState slotGetCameraStatus(QString);

private:
    Ui::CameraSettingPage *ui;
    int m_selected_camera = -1;

    bool eventFilter(QObject *wcg, QEvent *event);
    void changeSelectedCamera(int index);
    void createCameraSettingWidget(const QString &camera_name, const std::string &camera_id, int camera_index,
                                   common::CameraState state);
    bool deleteDisconnectedCamera(const std::string &);
};
}  // namespace vtk::display
Q_DECLARE_METATYPE(vtk::display::CameraItem::ItemData)
#endif  // CAMERASETTINGPAGE_H
