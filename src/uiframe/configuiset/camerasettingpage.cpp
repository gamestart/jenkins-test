/****************************************************************************
 *  @file     camerasettingpage.cpp
 *  @brief    设置窗口相机设置界面类
 *  @author
 *  @email
 *  @version
 *  @date     2020.12.21
 *  Copyright (c) 2020 SmartMore All rights reserved.
 ****************************************************************************/
#include "camerasettingpage.h"
#include "../commonuiset/toastwidget/toastwidget.h"
#include "cameraservice/cameramanager.h"
#include "configdatamanager/configmanager.h"
#include "ui_camerasettingpage.h"

#include <QAction>
#include <QDebug>
#include <QFile>

namespace vtk::display
{
using CameraManager = vtk::cameraservice::CameraManager;
using ConfigManager = vtk::config::ConfigManager;
using CameraConfigData = vtk::config::CameraConfigData;
using CameraConfigItem = vtk::config::CameraConfigItem;
using CameraState = vtk::common::CameraState;
using CameraStatusGetter = vtk::common::CameraStatusGetter;

CameraItem::CameraItem(CameraState state, const QString &camera_name, QWidget *parent) : QWidget(parent)
{
    setObjectName("CameraItem");
    setStyleSheet(QString::fromUtf8("#CameraItem{border-bottom: 1px solid #eeeeee;background:transparent;}"));
    auto horizontalLayout = new QHBoxLayout(this);
    horizontalLayout->setContentsMargins(15, 0, 15, 0);
    horizontalLayout->setSpacing(5);
    m_camera_status_lab = new QLabel(this);
    m_camera_status_lab->setObjectName(QString::fromUtf8("camera_status_lab"));
    m_camera_status_lab->setAlignment(Qt::AlignCenter);
    m_camera_status_lab->setMaximumSize(QSize(8, 8));
    horizontalLayout->addWidget(m_camera_status_lab);

    m_name_lab = new QLabel;
    m_name_lab->setText(camera_name);
    m_name_lab->setStyleSheet(QString::fromUtf8("border: 0px; color: rgba(0, 0, 0, 0.80); font-size: 14px;"));
    horizontalLayout->addWidget(m_name_lab);

    m_switch_btn = new QToolButton;
    m_switch_btn->setStyleSheet(QString::fromUtf8("QToolButton{border: 0px;}"));
    m_switch_btn->setCheckable(true);
    QIcon icon;
    icon.addFile(QString::fromUtf8(":/uiframe/images/camera_config/off.svg"), QSize(), QIcon::Normal, QIcon::Off);
    icon.addFile(QString::fromUtf8(":/uiframe/images/camera_config/on.svg"), QSize(), QIcon::Normal, QIcon::On);
    m_switch_btn->setIcon(icon);
    m_switch_btn->setIconSize(QSize(24, 14));
    horizontalLayout->addWidget(m_switch_btn);

    if (m_switch_btn->isChecked())
    {
        m_switch_btn->setToolTip(tr("Close Camera"));
    }
    else
    {
        m_switch_btn->setToolTip(tr("Open Camera"));
    }
    connect(m_switch_btn, &QToolButton::clicked, this, [&](bool checked) { emit signalSendCameraStatus(checked); });

    m_delete_menu = new QAction(tr("Delete"), this);
    addAction(m_delete_menu);
    connect(m_delete_menu, &QAction::triggered, this, [&]() { emit signalMenuAction(Action::DeleteCamera); });

    setContextMenuPolicy(Qt::ActionsContextMenu);

    setCameraStatus(state);
}

const QString CameraItem::getText() const
{
    return m_name_lab->text();
}

void CameraItem::setText(const QString &txt)
{
    m_name_lab->setText(txt);
}

void CameraItem::setCameraStatus(CameraState state)
{
    m_state = state;
    switch (state)
    {
        case CameraState::disconnect:
            m_camera_status_lab->setProperty("camera_status", "disconnect");
            m_switch_btn->setChecked(false);
            m_switch_btn->setEnabled(false);
            m_delete_menu->setEnabled(true);
            m_switch_btn->setToolTip(tr("Open Camera"));
            break;
        case CameraState::opening:
            m_camera_status_lab->setProperty("camera_status", "open");
            m_switch_btn->setChecked(true);
            m_switch_btn->setEnabled(true);
            m_switch_btn->setToolTip(tr("Close Camera"));
            m_delete_menu->setEnabled(false);
            break;
        case CameraState::closed:
            m_camera_status_lab->setProperty("camera_status", "close");
            m_switch_btn->setChecked(false);
            m_switch_btn->setEnabled(true);
            m_switch_btn->setToolTip(tr("Open Camera"));
            m_delete_menu->setEnabled(false);
            break;
    }
    style()->polish(m_camera_status_lab);
    m_camera_status_lab->update();
}

CameraSettingPage::CameraSettingPage(QWidget *parent) : QWidget(parent), ui(new Ui::CameraSettingPage)
{
    ui->setupUi(this);
    ui->camera_list_widget->viewport()->installEventFilter(this);

    connect(ui->refresh_btn, SIGNAL(clicked()), this, SLOT(slotRefreshCameraList()));
    connect(&CameraStatusGetter::getInstance(), &CameraStatusGetter::signalGetCameraStatus, this,
            &CameraSettingPage::slotGetCameraStatus);
}

CameraSettingPage::~CameraSettingPage()
{
    delete ui;
}

CameraState CameraSettingPage::slotGetCameraStatus(QString camera_name)
{
    for (int j = 0; j < ui->camera_list_widget->count(); j++)
    {
        auto item = ui->camera_list_widget->item(j);
        auto data = item->data(Qt::UserRole).value<CameraItem::ItemData>();
        auto name = QString::fromStdString(data.camera_name);
        if (name == camera_name)
        {
            return dynamic_cast<CameraItem *>(ui->camera_list_widget->itemWidget(item))->getCameraStatus();
        }
    }
    return CameraState::disconnect;
}

void CameraSettingPage::saveCameraConfigData()
{
    CameraConfigData data;
    for (int i = 0; i < ui->camera_settings->count(); i++)
    {
        CameraConfigItem data_item;
        data_item.name = dynamic_cast<CameraSetting *>(ui->camera_settings->widget(i))->getCameraName();
        data_item.id = dynamic_cast<CameraSetting *>(ui->camera_settings->widget(i))->getCameraId();
        data_item.trigger_source = dynamic_cast<CameraSetting *>(ui->camera_settings->widget(i))->getTriggerSource();
        data_item.trigger_activation =
            dynamic_cast<CameraSetting *>(ui->camera_settings->widget(i))->getTriggerActivation();
        data.push_back(data_item);
    }
    ConfigManager::getInstance().setCameraConfigData(data);
}

void CameraSettingPage::initCameraListWidget()
{
    const auto &camera_id_list = CameraManager::getInstance().getCameraIdList();
    ui->camera_list_widget->clear();
    for (int i = ui->camera_settings->count(); i >= 0; i--)
    {
        auto page_widget = ui->camera_settings->widget(i);
        ui->camera_settings->removeWidget(page_widget);
        delete page_widget;
    }
    QStringList camera_names;
    int camera_index = 0;
    CameraConfigData camera_data = ConfigManager::getInstance().getCameraConfigData();
    for (const auto &data : camera_data)
    {
        camera_names.append(data.name);
    }
    for (const auto &camera_id : camera_id_list)
    {
        QString name;
        for (int i = 0; i < camera_data.size(); i++)
        {
            if (camera_id == camera_data[i].id.toStdString())
            {
                name = camera_data[i].name;
                break;
            }
        }
        if (name.isEmpty())
        {
            int name_index = 1;
            while (true)
            {
                name = QString("Camera%1").arg(name_index);
                if (camera_names.contains(name))
                    name_index++;
                else
                    break;
            }
            camera_names.append(name);
        }
        auto device = CameraManager::getInstance().getCameraDevice(camera_id);
        if (!device)
            continue;
        createCameraSettingWidget(name, camera_id, camera_index,
                                  device->getCameraStatus() ? CameraState::opening : CameraState::closed);
        camera_index++;
    }
    for (auto &camera_id : CameraManager::getInstance().getDisconnectCameraIdList())
    {
        QString name;
        for (int i = 0; i < camera_data.size(); i++)
        {
            if (camera_id == camera_data[i].id.toStdString())
            {
                name = camera_data[i].name;
                break;
            }
        }
        createCameraSettingWidget(name, camera_id, camera_index, CameraState::disconnect);
        camera_index++;
    }
    connect(ui->camera_list_widget, &QListWidget::currentRowChanged, this,
            [&](int index) { changeSelectedCamera(index); });
    ui->camera_list_widget->setCurrentRow(0);
    ui->camera_settings->setCurrentIndex(0);
    saveCameraConfigData();
}

void CameraSettingPage::slotRefreshCameraList()
{
    ui->refresh_btn->blockSignals(true);
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
    for (int j = 0; j < ui->camera_list_widget->count(); j++)
    {
        auto item = ui->camera_list_widget->item(j);
        if (dynamic_cast<CameraItem *>(ui->camera_list_widget->itemWidget(item))->getCameraStatus() ==
            CameraState::opening)
        {
            ToastWidget::showTip(tr("Please close all cameras"), this);
            QApplication::restoreOverrideCursor();
            ui->refresh_btn->blockSignals(false);
            return;
        }
    }
    if (!validateCameraSettings())
    {
        ToastWidget::showErrorTip(tr("Please fix config errors"), this);
        QApplication::restoreOverrideCursor();
        ui->refresh_btn->blockSignals(false);
        return;
    }
    m_selected_camera = -1;
    CameraManager::getInstance().init();
    disableFocus();
    initCameraListWidget();
    QApplication::restoreOverrideCursor();
    ui->refresh_btn->blockSignals(false);
}

void CameraSettingPage::cancelCameraConfig()
{
    CameraConfigData data = ConfigManager::getInstance().getCameraConfigData();
    for (int i = 0; i < ui->camera_settings->count(); i++)
    {
        auto camera_setting = dynamic_cast<CameraSetting *>(ui->camera_settings->widget(i));
        if (camera_setting)
            camera_setting->changeTriggerMode();
        for (int j = 0; j < data.size(); j++)
        {
            if (camera_setting && camera_setting->getCameraId() == data[j].id)
            {
                camera_setting->setCameraName(data[j].name);
                break;
            }
        }
    }
}

void CameraSettingPage::changeSelectedCamera(int index)
{
    if (index == -1)
        return;
    // first validate current camera config ...
    if (m_selected_camera != -1 && m_selected_camera < ui->camera_settings->count() &&
        !dynamic_cast<CameraSetting *>(ui->camera_settings->widget(m_selected_camera))->validateCameraConfig())
    {
        ui->camera_list_widget->setCurrentRow(m_selected_camera);
        return;
    }

    m_selected_camera = index;

    // show corresponding stacked widget page
    ui->camera_settings->setCurrentIndex(index);
}

void CameraSettingPage::createCameraSettingWidget(const QString &camera_name, const std::string &camera_id,
                                                  int camera_index, common::CameraState state)
{
    auto item = new QListWidgetItem;
    auto widget = new CameraItem(state, camera_name);
    CameraItem::ItemData data{camera_name.toStdString(), camera_id};
    item->setData(Qt::UserRole, QVariant::fromValue(data));
    item->setSizeHint(QSize(138, 35));
    ui->camera_list_widget->addItem(item);
    ui->camera_list_widget->setItemWidget(item, widget);

    auto new_page = new CameraSetting(this, data.camera_name, data.camera_id);

    connect(widget, &CameraItem::signalMenuAction, this, [=](CameraItem::Action action) {
        switch (action)
        {
            case CameraItem::Action::DeleteCamera:
                if (deleteDisconnectedCamera(camera_id))
                {
                    ui->camera_list_widget->clearFocus();
                    delete item;
                    delete new_page;
                }
                else
                {
                    // 相机被使用
                    vtk::common::Message::postWarningMessage(
                        tr("The camera is in use, please modify the corresponding image source settings first"));
                }
                break;
        }
    });

    connect(new_page, SIGNAL(signalCheckDuplicate(const std::string &)), this,
            SLOT(slotCheckDuplicate(const std::string &)));
    connect(widget, &CameraItem::signalSendCameraStatus, new_page, [=](bool is_open) {
        if (validateCameraSettings())  // 涉及保存所以检查所有页面配置
        {
            ui->camera_list_widget->setCurrentRow(camera_index);
            is_open = new_page->changeCameraStatus(is_open);
            if (item)
            {
                auto camera_item = dynamic_cast<CameraItem *>(ui->camera_list_widget->itemWidget(item));
                if (camera_item)
                    camera_item->setCameraStatus(is_open ? CameraState::opening : CameraState::closed);
            }
        }
        else
        {
            is_open = !is_open;
            if (item)
            {
                auto camera_item = dynamic_cast<CameraItem *>(ui->camera_list_widget->itemWidget(item));
                if (camera_item)
                    camera_item->setCameraStatus(is_open ? CameraState::opening : CameraState::closed);
            }
            ToastWidget::showErrorTip(tr("Fix config errors"), this);
        }
    });
    connect(new_page, &CameraSetting::signalCameraNameChange, this, [=](const std::string &name) {
        if (item)
        {
            auto camera_item = dynamic_cast<CameraItem *>(ui->camera_list_widget->itemWidget(item));
            if (camera_item)
                camera_item->setText(QString::fromStdString(name));
            auto data = item->data(Qt::UserRole).value<CameraItem::ItemData>();
            data.camera_name = name;
            item->setData(Qt::UserRole, QVariant::fromValue(data));
            saveCameraConfigData();
        }
    });
    connect(&CameraManager::getInstance(), &CameraManager::signalCameraStatusChange, new_page,
            [=](const std::string &signal_camera_id, vtk::common::CameraState state) {
                if (signal_camera_id == camera_id)
                {
                    new_page->changeCameraStatus(state);
                    if (item)
                    {
                        auto camera_item = dynamic_cast<CameraItem *>(ui->camera_list_widget->itemWidget(item));
                        if (camera_item)
                            camera_item->setCameraStatus(state);
                    }
                }
            });
    ui->camera_settings->addWidget(new_page);
}

bool CameraSettingPage::deleteDisconnectedCamera(const std::string &camera_id)
{
    QString id = QString::fromStdString(camera_id);
    auto &source_list = ConfigManager::getInstance().getSourceConfigData();
    auto &camera_list = ConfigManager::getInstance().getCameraConfigData();
    for (const auto &source : source_list)
    {
        for (const auto &camera : camera_list)
        {
            if (source.source == camera.name && id == camera.id)
            {
                return false;
            }
        }
    }
    CameraManager::getInstance().deleteDisconnectedCamera(camera_id);
    return true;
}

std::vector<CameraData> CameraSettingPage::slotAddNewCamera()
{
    std::vector<CameraData> camera_datas;
    for (int i = 0; i < ui->camera_list_widget->count(); i++)
    {
        auto item = ui->camera_list_widget->item(i);
        auto data = item->data(Qt::UserRole).value<CameraItem::ItemData>();
        auto name = QString::fromStdString(data.camera_name);
        auto state = dynamic_cast<CameraItem *>(ui->camera_list_widget->itemWidget(item))->getCameraStatus();
        CameraData camera_data;
        camera_data.is_open = state == CameraState::opening;
        camera_data.camera_name = name;
        camera_datas.push_back(camera_data);
    }
    return camera_datas;
}

bool CameraSettingPage::slotCheckDuplicate(const std::string &camera_id)
{
    int index;
    for (int i = 0; i < ui->camera_settings->count(); i++)
    {
        if (dynamic_cast<CameraSetting *>(ui->camera_settings->widget(i))->getCameraId() ==
            QString::fromStdString(camera_id))
        {
            index = i;
            break;
        }
    }
    for (int i = 0; i < ui->camera_settings->count(); i++)
    {
        if (i == index)
            continue;
        if (dynamic_cast<CameraSetting *>(ui->camera_settings->widget(i))->getCameraName() ==
            dynamic_cast<CameraSetting *>(ui->camera_settings->widget(index))->getCameraName())
        {
            return true;
        }
    }
    return false;
}

bool CameraSettingPage::validateCameraSettings()
{
    for (int i = 0; i < ui->camera_settings->count(); i++)
    {
        if (!dynamic_cast<CameraSetting *>(ui->camera_settings->widget(i))->validateCameraConfig())
            return false;
    }
    return true;
}

void CameraSettingPage::disableFocus()
{
    for (int i = 0; i < ui->camera_settings->count(); i++)
    {
        for (int j = 1; j <= 5; j++)
        {
            dynamic_cast<CameraSetting *>(ui->camera_settings->widget(i))->disableFocus(j);
        }
    }
}

bool CameraSettingPage::eventFilter(QObject *wcg, QEvent *event)
{
    //相机配置不合法时禁止切换相机列表焦点
    if (wcg == ui->camera_list_widget->viewport())
    {
        if (event->type() == QEvent::MouseButtonPress || event->type() == QEvent::MouseButtonDblClick)
        {
            auto camera_setting = dynamic_cast<CameraSetting *>(ui->camera_settings->widget(m_selected_camera));
            if (m_selected_camera != -1 && camera_setting && !camera_setting->validateCameraConfig())
            {
                event->ignore();
                return true;
            }
        }
    }
    return QWidget::eventFilter(wcg, event);
}

}  // namespace vtk::display
