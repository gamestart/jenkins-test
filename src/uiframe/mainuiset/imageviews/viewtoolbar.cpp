/****************************************************************************
 *  @file     viewtoolbar.cpp
 *  @brief    图像视窗工具条类
 *  @author   junjie.zeng
 *  @email    junjie.zeng@smartmore.com
 *  @version
 *  @date     2020.12.21
 *  Copyright (c) 2020 SmartMore All rights reserved.
 ****************************************************************************/
#include "viewtoolbar.h"
#include "cameraservice/cameramanager.h"
#include "common/vtkcommon.h"
#include "configdatamanager/configmanager.h"
#include "ui_viewtoolbar.h"
#include "uiframe/commonuiset/messagedialog/messagedialog.h"

#include <QFileDialog>
#include <QStyle>
#include <QTextCodec>

namespace vtk::display
{
using ConfigManager = vtk::config::ConfigManager;
using CameraState = vtk::common::CameraState;
using ImageListForwarder = vtk::common::ImageListForwarder;
using CameraManager = vtk::cameraservice::CameraManager;
using TriggerMode = vtk::config::TriggerMode;

ViewToolBar::ViewToolBar(QWidget *parent) : QWidget(parent), ui(new Ui::ViewToolBar)
{
    init();
}

ViewToolBar::~ViewToolBar()
{
    delete ui;
}

void ViewToolBar::setViewInfo(const ImageView::ViewInfo &info)
{
    m_view_info = info;
    ui->view_name_lab->setText(m_view_info.view_name);
    QFontMetrics font_width(ui->view_name_lab->font());
    auto view_config_data = ConfigManager::getInstance().getViewConfigData();
    QString elide_note;
    foreach (const auto &view_data, view_config_data)
    {
        if (m_view_info.view_uuid == view_data.uuid)
        {
            if (view_data.source == "File")
                elide_note = font_width.elidedText(m_view_info.view_name, Qt::ElideRight, 70);
            else
                elide_note = font_width.elidedText(m_view_info.view_name, Qt::ElideRight, 60);
            break;
        }
    }
    ui->view_name_lab->setText(elide_note);
    if (elide_note != m_view_info.view_name)
        ui->view_name_lab->setToolTip(m_view_info.view_name);

    updateDeviceIcon();
    updateCameraStatus();
}

void ViewToolBar::setScalefactor(int factor)
{
    ui->scale_factor_lab->setText(QString("%1%").arg(static_cast<size_t>(factor)));
}

void ViewToolBar::init()
{
    initWidgets();
    initConnects();
}

void ViewToolBar::initWidgets()
{
    ui->setupUi(this);
    updateDeviceIcon();
    updateCameraStatus();
}

void ViewToolBar::initConnects()
{
    connect(&CameraManager::getInstance(), &CameraManager::signalCameraStatusChange, this,
            [&](const std::string &camera_id, CameraState state) {
                if (vtk::common::isSameCameraId(m_view_info.view_uuid, camera_id))
                    updateCameraStatus(state);
            });
}

void ViewToolBar::updateCameraStatus()
{
    auto camera_id = vtk::common::getCameraIdByViewId(m_view_info.view_uuid);
    if (camera_id.empty())
    {
        ui->device_btn->setEnabled(true);
        return;
    }
    const auto &camera_device = CameraManager::getInstance().getCameraDevice(camera_id);
    if (camera_device)
        updateCameraStatus(camera_device->getCameraStatus() ? CameraState::opening : CameraState::closed);
    else
        updateCameraStatus(CameraState::disconnect);
}

void ViewToolBar::updateCameraStatus(CameraState state)
{
    switch (state)
    {
        case CameraState::disconnect:
            ui->camera_status_lab->setProperty("camera_status", "disconnect");
            ui->camera_status_lab->setToolTip(tr("Warning"));
            ui->device_btn->setChecked(false);
            ui->device_btn->setEnabled(false);
            break;
        case CameraState::opening:
            ui->camera_status_lab->setProperty("camera_status", "open");
            ui->camera_status_lab->setToolTip(tr("Online"));
            ui->device_btn->setChecked(true);
            ui->device_btn->setEnabled(false);
            break;
        case CameraState::closed:
            ui->camera_status_lab->setProperty("camera_status", "close");
            ui->camera_status_lab->setToolTip(tr("Offline"));
            ui->device_btn->setChecked(false);
            ui->device_btn->setEnabled(true);
            break;
    }
    style()->polish(ui->camera_status_lab);
    ui->camera_status_lab->update();
}

void ViewToolBar::updateCameraStatusIconToolTip()
{
    if (ui->camera_status_lab->property("camera_status").toString() == "disconnect")
    {
        ui->camera_status_lab->setToolTip(tr("Warning"));
    }
    else if (ui->camera_status_lab->property("camera_status").toString() == "open")
    {
        ui->camera_status_lab->setToolTip(tr("Online"));
    }
    else if (ui->camera_status_lab->property("camera_status").toString() == "close")
    {
        ui->camera_status_lab->setToolTip(tr("Offline"));
    }
}

void ViewToolBar::updateDeviceIconToolTip()
{
    if (ui->device_btn->property("device_type").toString() == "local")
        ui->device_btn->setToolTip(tr("Upload File"));
    else
        ui->device_btn->setToolTip(tr("Open Camera"));
}

void ViewToolBar::updateDeviceIcon()
{
    auto view_config_data = ConfigManager::getInstance().getViewConfigData();
    foreach (const auto &view_data, view_config_data)
    {
        if (m_view_info.view_uuid == view_data.uuid)
        {
            view_data.source == "File" ? ui->device_btn->setProperty("device_type", "local") :
                                         ui->device_btn->setProperty("device_type", "camera");
            updateDeviceIconToolTip();
            ui->camera_status_lab->setVisible(view_data.source != "File");
            break;
        }
    }
    style()->polish(ui->device_btn);
    style()->polish(ui->camera_status_lab);
}

void ViewToolBar::resetAcquisitionControl(const std::string &camera_id)
{
    const auto &trigger_mode = ConfigManager::getInstance().getTriggerMode();
    const auto &camera_device = CameraManager::getInstance().getCameraDevice(camera_id);
    if (trigger_mode == TriggerMode::softTrigger)
    {
        camera_device->setTriggerSource(smartmore::TriggerSource::SoftType);
    }
    else
    {
        const auto &camera_data = ConfigManager::getInstance().getCameraConfigData();
        for (const auto &data : camera_data)
        {
            if (data.id == QString::fromStdString(camera_id) && data.trigger_source != -1 &&
                data.trigger_activation != -1)
            {
                camera_device->setTriggerSource(static_cast<smartmore::TriggerSource>(data.trigger_source));
                camera_device->setTriggerActivation(static_cast<smartmore::TriggerActivation>(data.trigger_activation));
                break;
            }
        }
    }
}

void ViewToolBar::on_device_btn_clicked()
{
    if (ui->device_btn->property("device_type").toString() == "local")  //本地设备导入
    {
        QFileDialog file_dialog;
        auto file_path_list =
            file_dialog.getOpenFileNames(this, "Open Files", ".", tr("Image Files (*.png *.jpg *.bmp)"));
        QList<ImageListForwarder::ImageInfo> image_info_list;
        for (const auto &file_path : file_path_list)
        {
            image_info_list.append({file_path, QUuid::createUuid()});
        }
        emit ImageListForwarder::getInstance().signalPostImageListInfo(m_view_info.view_uuid, image_info_list);

        if (file_path_list.size() <= 0)
            return;

        auto file_path = file_path_list.at(0);
#ifdef Q_OS_WIN
        QTextCodec *gbk_code = QTextCodec::codecForName("GBK");
        auto path = std::string(gbk_code->fromUnicode(file_path).data());
#else
        auto path = file_path.toStdString();
#endif
        auto input_image = cv::imread(std::string(path));
        emit signalUpdateImage(input_image);
    }
    else  //相机设备
    {
        auto camera_id = vtk::common::getCameraIdByViewId(m_view_info.view_uuid);
        if (!camera_id.empty())
        {
            const auto &camera_device = CameraManager::getInstance().getCameraDevice(camera_id);
            if (camera_device)
            {
                ui->device_btn->setEnabled(false);
                if (camera_device->openCamera())
                {
                    // 打开失败
                    ui->device_btn->setChecked(false);
                    ui->device_btn->setEnabled(true);
                    MessageDialog message_box_dialog(DialogType::WarningDialog, tr("open failed"));
                    message_box_dialog.exec();
                }
                resetAcquisitionControl(camera_id);
                return;
            }
        }
        MessageDialog message_box_dialog(DialogType::WarningDialog, tr("unknown camera"));
        message_box_dialog.exec();
    }
}

void vtk::display::ViewToolBar::on_image_dispaly_btn_toggled(bool checked)
{
    emit signalChangeDisplayImageType(!checked);
}

void ViewToolBar::on_zoomout_btn_clicked()
{
    emit signalZoomoutImage();
}

void ViewToolBar::on_zoomin_btn_clicked()
{
    emit signalZoominImage();
}

void ViewToolBar::on_revert_btn_clicked()
{
    emit signalRevertImage();
}

void ViewToolBar::changeEvent(QEvent *e)  //重写的事件处理方法
{
    QWidget::changeEvent(e);  //让基类执行事件处理方法
    switch (e->type())
    {
        case QEvent::LanguageChange:  //如果是语言改变事件
            if (ui)
                ui->retranslateUi(this);  //更新UI的语言
            updateDeviceIconToolTip();
            updateCameraStatusIconToolTip();
            break;
        default:
            break;
    }
}

}  // namespace vtk::display
