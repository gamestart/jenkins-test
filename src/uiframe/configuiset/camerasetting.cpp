/****************************************************************************
 *  @file     camerasetting.cpp
 *  @brief    设置窗口单个相机设置界面类
 *  @author
 *  @email
 *  @version
 *  @date     2020.12.21
 *  Copyright (c) 2020 SmartMore All rights reserved.
 ****************************************************************************/
#include "camerasetting.h"
#include "cameraservice/cameramanager.h"
#include "configdatamanager/configmanager.h"
#include "ui_camerasetting.h"
#include "uiframe/commonuiset/messagedialog/messagedialog.h"
#include "uiframe/commonuiset/toastwidget/toastwidget.h"

#include <QDateTime>
#include <QDesktopServices>
#include <QDir>
#include <QGraphicsPixmapItem>
#include <QProcess>
#include <QScrollBar>
#include <QTextCodec>
#include <QWheelEvent>

namespace vtk::display
{
using CameraManager = vtk::cameraservice::CameraManager;
using ConfigManager = vtk::config::ConfigManager;
using TriggerMode = vtk::config::TriggerMode;
using IPInfo = vtk::cameraservice::CameraDevice::IPInfo;

CameraSetting::CameraSetting(QWidget *parent, const std::string &camera_name, const std::string &camera_id)
    : QWidget(parent), ui(new Ui::CameraSetting), m_camera_name(camera_name), m_camera_id(camera_id)
{
    init();
}

CameraSetting::~CameraSetting()
{
    stopCapture();
    delete ui;
}

void CameraSetting::init()
{
    initWidgets();
    initWarnings();
    initConnections();
    m_tip_widget = new TipWidget("", "://uiframe/images/camera_config/loading.gif", ui->camera_stream);
    m_tip_widget->hide();
}

void CameraSetting::initWidgets()
{
    ui->setupUi(this);
    ui->retranslateUi(this);

    ui->camera_name_input->setText(QString::fromStdString(m_camera_name));
    ui->camera_id_input->setText(QString::fromStdString(m_camera_id));
    ui->camera_id_input->setEnabled(false);
    ui->gamma_input->setEnabled(false);
    ui->gamma_title->setEnabled(false);

    const auto &camera_device = CameraManager::getInstance().getCameraDevice(m_camera_id);
    if (camera_device)
    {
        m_is_open = camera_device->getCameraStatus();
        updateDisplay(m_is_open);
    }
    else
    {
        // 不在线的相机
        updateDisplay(vtk::common::CameraState::disconnect);
    }

    updateIP();
    updateSubnetmask();

    ui->stream_btn->setToolTip(tr("Start Acquisition"));
}

void CameraSetting::initWarnings()
{
    ui->camera_ip_warning->setText(tr("Please input valid IP address."));
    ui->camera_subnet_mask_warning->setText(tr("Please input valid subnet mask."));

    ui->camera_name_warning->hide();
    ui->camera_ip_warning->hide();
    ui->camera_subnet_mask_warning->hide();
    ui->camera_exposure_warning->setText("");
    ui->camera_gain_warning->setText("");
    ui->camera_gamma_warning->setText("");
}

void CameraSetting::initConnections()
{
    connect(ui->camera_name_input, &QLineEdit::editingFinished, this, [=] { disableFocus(1); });
    ui->camera_name_input->installEventFilter(this);
    connect(ui->exposure_input, &QLineEdit::editingFinished, this, [=] { disableFocus(2); });
    ui->exposure_input->installEventFilter(this);
    connect(ui->gain_input, &QLineEdit::editingFinished, this, [=] { disableFocus(3); });
    ui->gain_input->installEventFilter(this);
    connect(ui->camera_ip_input, &QLineEdit::editingFinished, this, [=] { disableFocus(4); });
    ui->camera_ip_input->installEventFilter(this);
    connect(ui->subnet_mask_input, &QLineEdit::editingFinished, this, [=] { disableFocus(5); });
    ui->subnet_mask_input->installEventFilter(this);
    connect(ui->gamma_input, &QLineEdit::editingFinished, this, [=] { disableFocus(6); });
    ui->gamma_input->installEventFilter(this);
    connect(ui->stream_btn, &QToolButton::clicked, this, [&] {
        if (ui->stream_btn->isChecked())
        {
            ui->stream_btn->setToolTip(tr("Stop Acquisition"));
            m_tip_widget->setText(tr("Start Grabbing..."));
            CameraManager::getInstance().startCameraCapture(m_camera_id);
        }
        else
        {
            ui->stream_btn->setToolTip(tr("Start Acquisition"));
            m_tip_widget->setText(tr("Stop Grabbing..."));
            CameraManager::getInstance().stopCameraCapture(m_camera_id);
        }
    });

    connect(ui->catch_btn, &QToolButton::clicked, this, [&] {
        ui->catch_btn->setEnabled(false);
        //用userdata判断是否是由click触发的保存
        ui->catch_btn->setUserData(Qt::UserRole, new QObjectUserData());
    });
    connect(&CameraManager::getInstance(), &CameraManager::signalSendRealtimeImage, this,
            [&](const std::string &camera_id, const cv::Mat &cv_image) {
                if (m_camera_id == camera_id)
                {
                    if (!ui->catch_btn->isEnabled() && ui->catch_btn->userData(Qt::UserRole))
                    {
                        interceptImage(cv_image);
                        ui->catch_btn->setEnabled(true);
                        ui->catch_btn->setUserData(Qt::UserRole, nullptr);
                        ToastWidget::showSaveTip(tr("Picture saved successfully"),
                                                 ":/uiframe/images/camera_config/Success.svg", this);
                    }

                    ui->camera_stream_view->setImage(vtk::common::matToQImage(cv_image), false);
                }
            });
    connect(ui->camera_stream_view, &ImageGraphicsView::signalUpdateFactor, this,
            [&](float factor) { ui->scale_lab->setText(QString("%1%").arg(static_cast<size_t>(factor * 100))); });
    connect(ui->trigger_mode_combox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this,
            [&](int index) {
                MessageDialog message_box_dialog(
                    DialogType::WarningDialog,
                    tr("The camera trigger mode must be the same. Switching this "
                       "camera trigger mode will switch other trigger modes at the same time!"),
                    {Confirm, Cancel});
                if (message_box_dialog.exec() == QDialog::Accepted)
                {
                    ConfigManager::getInstance().setTriggerMode(static_cast<TriggerMode>(index));
                    ui->stream_btn->setChecked(false);
                    CameraManager::getInstance().stopCameraCapture(m_camera_id);
                }
                else
                {
                    updateTriggerModeCombox();
                }
            });
    connect(&ConfigManager::getInstance(), &ConfigManager::signalTriggerModeChanged, this,
            [&] { changeTriggerMode(); });
    connect(ui->trigger_source_combox, &QComboBox::currentTextChanged, this, [&](const QString &trigger_source) {
        const auto trigger_source_value = ui->trigger_source_combox->currentData().toInt();
        const auto &camera_device = CameraManager::getInstance().getCameraDevice(m_camera_id);
        if (camera_device)
            camera_device->setTriggerSource(static_cast<smartmore::TriggerSource>(trigger_source_value));
    });
    connect(ui->trigger_activation_combox, &QComboBox::currentTextChanged, this,
            [&](const QString &trigger_activation) {
                const auto trigger_activation_value = ui->trigger_activation_combox->currentData().toInt();
                const auto &camera_device = CameraManager::getInstance().getCameraDevice(m_camera_id);
                if (camera_device)
                    camera_device->setTriggerActivation(
                        static_cast<smartmore::TriggerActivation>(trigger_activation_value));
            });
}

void CameraSetting::updateDisplay(vtk::common::CameraState state)
{
    bool is_open = state == vtk::common::CameraState::opening;
    ui->exposure_input->setEnabled(is_open);
    ui->exposure_title->setEnabled(is_open);
    ui->gain_input->setEnabled(is_open);
    ui->gain_title->setEnabled(is_open);
    const auto &camera_device = CameraManager::getInstance().getCameraDevice(m_camera_id);
    if (camera_device && camera_device->getDeviceInfo().device_type != smartmore::DeviceType::Dalsa)
    {
        ui->gamma_input->setEnabled(is_open);
        ui->gamma_title->setEnabled(is_open);
    }
    ui->trigger_mode_title->setEnabled(is_open);
    ui->trigger_mode_combox->setEnabled(is_open);
    ui->trigger_source_title->setEnabled(is_open);
    ui->trigger_source_combox->setEnabled(is_open);
    ui->trigger_activation_title->setEnabled(is_open);
    ui->trigger_activation_combox->setEnabled(is_open);
    ui->stream_btn->setEnabled(is_open);
    ui->catch_btn->setEnabled(is_open);
    ui->zoom_in_btn->setEnabled(is_open);
    ui->zoom_out_btn->setEnabled(is_open);
    ui->revert_btn->setEnabled(is_open);
    ui->camera_name_input->setEnabled(!is_open);
    ui->camera_name_title->setEnabled(!is_open);

    bool is_ip_modifiable = state == vtk::common::CameraState::closed;
    ui->camera_ip_input->setEnabled(is_ip_modifiable);
    ui->subnet_mask_input->setEnabled(is_ip_modifiable);
    ui->camera_ip_title->setEnabled(is_ip_modifiable);
    ui->subnet_mask_title->setEnabled(is_ip_modifiable);

    if (is_open)
    {
        updateExposure();
        updateGain();
        updateGamma();
        changeTriggerMode();
    }
    else
    {
        updateTriggerModeCombox();
    }
}

void CameraSetting::updateExposure()
{
    QString exposure_text = "0.00";
    const auto &camera_device = CameraManager::getInstance().getCameraDevice(m_camera_id);
    if (camera_device)
    {
        exposure_text = QString::asprintf("%.2f", camera_device->getExposureTime());
    }
    ui->exposure_input->setText(exposure_text);
}

void CameraSetting::updateGamma()
{
    const auto &camera_device = CameraManager::getInstance().getCameraDevice(m_camera_id);
    if (camera_device && camera_device->getDeviceInfo().device_type != smartmore::DeviceType::Dalsa)
    {
        ui->gamma_input->setText(QString::asprintf("%.2f", camera_device->getGamma()));
    }
}
void CameraSetting::changeTriggerMode()
{
    const auto &trigger_mode = ConfigManager::getInstance().getTriggerMode();
    const auto &camera_device = CameraManager::getInstance().getCameraDevice(m_camera_id);
    if (trigger_mode == TriggerMode::softTrigger)
    {
        ui->trigger_source_combox->setEnabled(false);
        ui->trigger_activation_combox->setEnabled(false);
        if (camera_device)
        {
            const auto current_trigger_activation_value = camera_device->getTriggerActivation();
            if (current_trigger_activation_value >= smartmore::TriggerActivation::LevelHigh)
            {
                camera_device->setTriggerActivation(smartmore::TriggerActivation::RisingEdge);
            }
            camera_device->setTriggerSource(smartmore::TriggerSource::SoftType);
        }
    }
    else
    {
        resetImageSourcesTriggerLevelAndOrder();
        ui->trigger_source_combox->setEnabled(true);
        ui->trigger_activation_combox->setEnabled(true);
        updateAvailableTriggerSource();
        if (camera_device)
        {
            const auto current_trigger_source_value = ui->trigger_source_combox->currentData().toInt();
            camera_device->setTriggerSource(static_cast<smartmore::TriggerSource>(current_trigger_source_value));
        }
        updateAvailableTriggerActivation();
    }
    updateTriggerModeCombox();
}

void CameraSetting::resetImageSourcesTriggerLevelAndOrder()
{
    const auto &source_data_list = ConfigManager::getInstance().getSourceConfigData();
    vtk::config::SourceConfigData datas;
    foreach (auto source_data, source_data_list)
    {
        source_data.trigger_level = "L0";
        source_data.trigger_level_order = source_data.trigger_level_order.replace(0, 2, "L0");
        datas.append(std::move(source_data));
    }
    ConfigManager::getInstance().setSourceConfigData(datas);
}

void CameraSetting::updateAvailableTriggerSource()
{
    if (ui->trigger_source_combox->count() != 0)
        return;

    ui->trigger_source_combox->blockSignals(true);
    std::vector<int> values;
    const auto &camera_device = CameraManager::getInstance().getCameraDevice(m_camera_id);
    if (!camera_device)
        return;

    camera_device->getAvailableTriggerSource(values);
    for (const auto value : values)
    {
        switch (value)
        {
            case 0:
            case 1:
            case 2:
            case 3:
                ui->trigger_source_combox->addItem(tr("Line %1").arg(value), value);
                break;
            default:
                break;
        }
    }

    auto current_trigger_source_value = getCurrentTriggerSourceValueFromConfigdata();
    if (current_trigger_source_value == -1)
        current_trigger_source_value = camera_device->getTriggerSource();
    const auto index = ui->trigger_source_combox->findData(current_trigger_source_value);
    if (index != -1)
        ui->trigger_source_combox->setCurrentIndex(index);
    ui->trigger_source_combox->blockSignals(false);
}

void CameraSetting::updateAvailableTriggerActivation()
{
    if (ui->trigger_activation_combox->count() != 0)
        return;

    ui->trigger_activation_combox->blockSignals(true);
    std::vector<int> values;
    const auto &camera_device = CameraManager::getInstance().getCameraDevice(m_camera_id);
    if (!camera_device)
        return;

    camera_device->getAvailableTriggerActivation(values);
    for (const auto value : values)
    {
        switch (value)
        {
            case 0:
                ui->trigger_activation_combox->addItem(tr("Rising Edge"), value);
                break;
            case 1:
                ui->trigger_activation_combox->addItem(tr("Falling Edge"), value);
                break;
            case 2:
                ui->trigger_activation_combox->addItem(tr("Level High"), value);
                break;
            case 3:
                ui->trigger_activation_combox->addItem(tr("Level Low"), value);
                break;
            default:
                break;
        }
    }

    auto current_trigger_activation_value = getCurrentTriggerActivationValueFromConfigdata();
    if (current_trigger_activation_value == -1)
        current_trigger_activation_value = camera_device->getTriggerActivation();
    const auto index = ui->trigger_activation_combox->findData(current_trigger_activation_value);
    if (index != -1)
        ui->trigger_activation_combox->setCurrentIndex(index);
    ui->trigger_activation_combox->blockSignals(false);
}

void CameraSetting::updateTriggerModeCombox()
{
    ui->trigger_mode_combox->blockSignals(true);
    const auto &trigger_mode = ConfigManager::getInstance().getTriggerMode();
    ui->trigger_mode_combox->setCurrentIndex(static_cast<int>(trigger_mode));
    ui->trigger_mode_combox->blockSignals(false);
}

int CameraSetting::getCurrentTriggerSourceValueFromConfigdata()
{
    const auto &camera_data = ConfigManager::getInstance().getCameraConfigData();
    for (const auto &data : camera_data)
    {
        if (data.id == QString::fromStdString(m_camera_id))
        {
            return data.trigger_source;
            break;
        }
    }
    return -1;
}

int CameraSetting::getCurrentTriggerActivationValueFromConfigdata()
{
    const auto &camera_data = ConfigManager::getInstance().getCameraConfigData();
    for (const auto &data : camera_data)
    {
        if (data.id == QString::fromStdString(m_camera_id))
        {
            return data.trigger_activation;
            break;
        }
    }
    return -1;
}

void CameraSetting::updateGain()
{
    QString gain_text = "0.00";
    const auto &camera_device = CameraManager::getInstance().getCameraDevice(m_camera_id);
    if (camera_device)
    {
        gain_text = QString::asprintf("%.2f", camera_device->getGain());
    }
    ui->gain_input->setText(gain_text);
}

void CameraSetting::updateIP()
{
    const auto &camera_device = CameraManager::getInstance().getCameraDevice(m_camera_id);
    if (camera_device)
    {
        auto ip_info = camera_device->getCameraIP();
        char ip_address[32] = "";
        sprintf(ip_address, "%d.%d.%d.%d", ip_info.ip >> 24, (ip_info.ip & 0xFF0000) >> 16, (ip_info.ip & 0xFF00) >> 8,
                ip_info.ip & 0xFF);
        ui->camera_ip_input->setText(QString(QLatin1String(ip_address)));
    }
    else
    {
        ui->camera_ip_input->setText(QString("0.0.0.0"));
    }
}

void CameraSetting::updateSubnetmask()
{
    const auto &camera_device = CameraManager::getInstance().getCameraDevice(m_camera_id);
    if (camera_device)
    {
        auto ip_info = camera_device->getCameraIP();
        char subnetmask[32] = "";
        sprintf(subnetmask, "%d.%d.%d.%d", ip_info.subnetmask >> 24, (ip_info.subnetmask & 0xFF0000) >> 16,
                (ip_info.subnetmask & 0xFF00) >> 8, ip_info.subnetmask & 0xFF);
        ui->subnet_mask_input->setText(QString(QLatin1String(subnetmask)));
    }
    else
    {
        ui->subnet_mask_input->setText(QString("0.0.0.0"));
    }
}

void CameraSetting::openCamera()
{
    const auto &camera_device = CameraManager::getInstance().getCameraDevice(m_camera_id);
    if (camera_device && !camera_device->openCamera())
    {
        m_is_open = true;
    }
    else
    {
        MessageDialog message_box_dialog(DialogType::WarningDialog,
                                         tr("Camera is not exist, please refresh camera list!"));
        message_box_dialog.exec();
    }
}

void CameraSetting::closeCamera()
{
    stopCapture();
    const auto &camera_device = CameraManager::getInstance().getCameraDevice(m_camera_id);
    if (camera_device)
    {
        camera_device->closeCamera();
    }
    m_is_open = false;
    // clear image item
    QImage image;
    ui->camera_stream_view->setImage(image, true);
}

QString CameraSetting::getCameraName()
{
    return ui->camera_name_input->text();
}

QString CameraSetting::getCameraId()
{
    return ui->camera_id_input->text();
}

int CameraSetting::getTriggerSource()
{
    const auto &current_data = ui->trigger_source_combox->currentData();
    return current_data.isValid() ? ui->trigger_source_combox->currentData().toInt() : -1;
}

int CameraSetting::getTriggerActivation()
{
    const auto &current_data = ui->trigger_activation_combox->currentData();
    return current_data.isValid() ? ui->trigger_activation_combox->currentData().toInt() : -1;
}

bool CameraSetting::validateCameraName()
{
    if (ui->camera_name_input->isEmpty())
    {
        ui->camera_name_warning->setText(tr("Please enter the camera name."));
        ui->camera_name_warning->show();
        return false;
    }
    if (ui->camera_name_input->exceedLength(20))
    {
        ui->camera_name_warning->setText(tr("The name length is between 1 and 20 characters."));
        ui->camera_name_warning->show();
        return false;
    }
    bool is_duplicate = emit signalCheckDuplicate(m_camera_id);
    if (is_duplicate)
    {
        ui->camera_name_warning->setText(tr("Camera name can't be duplicate."));
        ui->camera_name_warning->show();
        return false;
    }
    ui->camera_name_warning->hide();
    return true;
}

bool CameraSetting::validateCameraIp()
{
    QRegExp reg_exp_ip("((25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[0-9])[\\.]){3}(25[0-5]|2[0-4][0-9]|1[0-9][0-9]|["
                       "1-9][0-9]|[0-9])");
    if (!reg_exp_ip.exactMatch(ui->camera_ip_input->text()))
    {
        ui->camera_ip_warning->show();
        return false;
    }
    ui->camera_ip_warning->hide();
    return true;
}

bool CameraSetting::validateCameraSubnetMask()
{
    QRegExp reg_exp_ip("((25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[0-9])[\\.]){3}(25[0-5]|2[0-4][0-9]|1[0-9][0-9]|["
                       "1-9][0-9]|[0-9])");
    if (!reg_exp_ip.exactMatch(ui->subnet_mask_input->text()))
    {
        ui->camera_subnet_mask_warning->show();
        return false;
    }
    ui->camera_subnet_mask_warning->hide();
    return true;
}

bool CameraSetting::validateCameraExposure()
{
    QString exposure = ui->exposure_input->text();
    bool flag;
    double exposure_number = exposure.toDouble(&flag);
    if (!flag || exposure_number < 38 || exposure_number > 9999500)
    {
        ui->camera_exposure_warning->setText(tr("Range: 38-9999500"));
        return false;
    }
    ui->camera_exposure_warning->setText("");
    return true;
}

bool CameraSetting::validateCameraGain()
{
    QString gain = ui->gain_input->text();
    bool flag;
    double gain_number = gain.toDouble(&flag);
    if (!flag || gain_number < 0 || gain_number > 19.99)
    {
        ui->camera_gain_warning->setText(tr("Range: 0.00-19.99"));
        return false;
    }
    ui->camera_gain_warning->setText("");
    return true;
}

bool CameraSetting::validateCameraGamma()
{
    QString gamma = ui->gamma_input->text();
    if (gamma.isEmpty())
        return true;
    bool flag;
    double gamma_number = gamma.toDouble(&flag);
    if (!flag || gamma_number < 0 || gamma_number > 4)
    {
        ui->camera_gamma_warning->setText(tr("Range: 0.00-4.00"));
        return false;
    }
    ui->camera_gamma_warning->setText("");
    return true;
}

void CameraSetting::interceptImage(const cv::Mat &cv_image)
{
    auto time = QDateTime::currentDateTime().toString("hh-mm-ss");
    auto save_path = QApplication::applicationDirPath() + "/images/";
    QDir dir(save_path);
    if (!dir.exists())
        dir.mkdir(save_path);

    auto file_name = save_path.toStdString() + time.toStdString() + ".jpg";
#ifdef Q_OS_WIN
    auto gbk_code = QTextCodec::codecForName("GBK");
    file_name = std::string(gbk_code->fromUnicode(QString::fromStdString(file_name).data()));
#endif
    cv::imwrite(file_name, cv_image);
}

bool CameraSetting::validateCameraConfig()
{
    int flag = 0;
    if (!validateCameraName())
    {
        flag++;
    }
    if (!validateCameraIp())
    {
        flag++;
    }
    if (!validateCameraSubnetMask())
    {
        flag++;
    }
    if (m_is_open && !validateCameraExposure())
    {
        flag++;
    }
    if (m_is_open && !validateCameraGain())
    {
        flag++;
    }
    if (m_is_open && !validateCameraGamma())
    {
        flag++;
    }
    if (flag)
    {
        return false;
    }
    return true;
}

void CameraSetting::setCameraName(QString name)
{
    ui->camera_name_input->setText(name);
    emit signalCameraNameChange(name.toStdString());
}

void CameraSetting::disableFocus(int index)
{
    if (index == 1)
    {
        if (validateCameraName())
        {
            m_camera_name = ui->camera_name_input->text().toStdString();
            emit signalCameraNameChange(m_camera_name);
        }
        ui->camera_name_input->clearFocus();
    }
    else if (index == 2)
    {
        if (m_is_open && validateCameraExposure())
        {
            auto exposure_value = ui->exposure_input->text().toFloat();
            const auto &camera_device = CameraManager::getInstance().getCameraDevice(m_camera_id);
            if (camera_device)
            {
                camera_device->setExposureTime(exposure_value);
            }
        }
        ui->exposure_input->clearFocus();
    }
    else if (index == 3)
    {
        if (m_is_open && validateCameraGain())
        {
            auto gain_value = ui->gain_input->text().toFloat();
            const auto &camera_device = CameraManager::getInstance().getCameraDevice(m_camera_id);
            if (camera_device)
            {
                camera_device->setGain(gain_value);
            }
        }
        ui->gain_input->clearFocus();
    }
    else if (index == 4)
    {
        if (validateCameraIp() && ui->camera_ip_input->hasFocus())
        {
            const auto &camera_device = CameraManager::getInstance().getCameraDevice(m_camera_id);
            if (camera_device)
            {
                auto ip_str = ui->camera_ip_input->text();
                size_t temp[4] = {0};
                sscanf(ip_str.toLatin1().data(), "%d.%d.%d.%d", &temp[0], &temp[1], &temp[2], &temp[3]);
                unsigned int ip = 0;
                for (size_t i = 0; i < 4; i++)
                    ip += (temp[i] << (24 - (i * 8)) & 0xFFFFFFFF);
                auto ip_info = camera_device->getCameraIP();
                //判断ip是否改变
                if (ip != ip_info.ip)
                {
                    if (isOccupiedOfCurrentIP(ip_str))
                    {
                        updateIP();
                        return;
                    }
                    ip_info.ip = ip;
                    camera_device->setCameraIP(ip_info);
                    camera_device->initCamera();
                }
            }
            ui->camera_ip_input->clearFocus();
        }
    }
    else if (index == 5)
    {
        if (validateCameraSubnetMask() && ui->subnet_mask_input->hasFocus())
        {
            const auto &camera_device = CameraManager::getInstance().getCameraDevice(m_camera_id);
            if (camera_device)
            {
                auto subnetmask_str = ui->subnet_mask_input->text();
                size_t temp[4] = {0};
                sscanf(subnetmask_str.toLatin1().data(), "%d.%d.%d.%d", &temp[0], &temp[1], &temp[2], &temp[3]);
                unsigned int subnetmask = 0;
                for (size_t i = 0; i < 4; i++)
                    subnetmask += (temp[i] << (24 - (i * 8)) & 0xFFFFFFFF);
                auto ip_info = camera_device->getCameraIP();
                //判断subnetmask是否改变
                if (subnetmask != ip_info.subnetmask)
                {
                    ip_info.subnetmask = subnetmask;
                    camera_device->setCameraIP(ip_info);
                    camera_device->initCamera();
                }
            }
            ui->subnet_mask_input->clearFocus();
        }
    }
    else if (index == 6)
    {
        if (m_is_open && validateCameraGamma())
        {
            auto gamma_value = ui->gamma_input->text().toFloat();
            const auto &camera_device = CameraManager::getInstance().getCameraDevice(m_camera_id);
            if (camera_device)
            {
                camera_device->setGamma(gamma_value);
            }
        }
        ui->gamma_input->clearFocus();
    }
}

void CameraSetting::stopCapture()
{
    if (ui->stream_btn->isChecked())
    {
        ui->stream_btn->setChecked(false);
        CameraManager::getInstance().stopCameraCapture(m_camera_id);
    }
}

bool CameraSetting::eventFilter(QObject *wcg, QEvent *event)
{
    if (event->type() == QEvent::FocusIn)
    {
        if (wcg == ui->camera_name_input)
            ui->camera_name_warning->hide();
        else if (wcg == ui->camera_ip_input)
            ui->camera_ip_warning->hide();
        else if (wcg == ui->subnet_mask_input)
            ui->camera_subnet_mask_warning->hide();
        else if (wcg == ui->exposure_input)
            ui->camera_exposure_warning->setText("");
        else if (wcg == ui->gain_input)
            ui->camera_gain_warning->setText("");
    }

    return QWidget::eventFilter(wcg, event);
}

bool CameraSetting::isOccupiedOfCurrentIP(const QString &ip)
{
    if (common::isIpReachable(ip, 500))
    {
        MessageDialog message_box_dialog(DialogType::WarningDialog, tr("The IP is already occupied!"));
        message_box_dialog.exec();
        return true;
    }
    return false;
}

void CameraSetting::on_zoom_out_btn_clicked()
{
    ui->camera_stream_view->zoomoutImage();
}

void CameraSetting::on_zoom_in_btn_clicked()
{
    ui->camera_stream_view->zoominImage();
}

void CameraSetting::on_revert_btn_clicked()
{
    ui->camera_stream_view->revertImage();
}

void CameraSetting::on_view_img_btn_clicked()
{
    auto save_path = QApplication::applicationDirPath() + "/images/";
    QDir dir(save_path);
    if (!dir.exists())
        dir.mkdir(save_path);
    auto url = "file:///" + save_path;
    QDesktopServices::openUrl(QUrl(url, QUrl::TolerantMode));
}

bool CameraSetting::changeCameraStatus(bool is_open)
{
    is_open ? openCamera() : closeCamera();
    updateDisplay(m_is_open);
    return m_is_open;
}

bool CameraSetting::changeCameraStatus(vtk::common::CameraState state)
{
    if (state != vtk::common::CameraState::disconnect)
    {
        return changeCameraStatus(state == vtk::common::CameraState::opening);
    }
    else
    {
        closeCamera();
        m_is_open = false;
        updateDisplay(state);
        return m_is_open;
    }
}

void CameraSetting::hideEvent(QHideEvent *event)
{
    stopCapture();
    QWidget::hideEvent(event);
}
}  // namespace vtk::display
