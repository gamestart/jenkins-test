/****************************************************************************
 *  @file     configdialog.cpp
 *  @brief    设置窗口类
 *  @author
 *  @email
 *  @version
 *  @date     2020.12.21
 *  Copyright (c) 2020 SmartMore All rights reserved.
 ****************************************************************************/
#include "configdialog.h"
#include "configdatamanager/configmanager.h"
#include "configtitlebar.h"
#include "ui_configdialog.h"
#include "uiframe/commonuiset/toastwidget/toastwidget.h"
#include "uiframe/commonuiset/waitingdialog/waitingdialog.h"

#include <QDebug>
#include <QFile>
#include <QGraphicsDropShadowEffect>

namespace vtk::display
{
ConfigDialog::ConfigDialog(QWidget *parent) : QDialog(parent), ui(new Ui::ConfigDialog)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    setWindowModality(Qt::ApplicationModal);

    // add box shadow
    this->setAttribute(Qt::WA_TranslucentBackground);
    QGraphicsDropShadowEffect *pEffect = new QGraphicsDropShadowEffect(ui->frame);
    pEffect->setOffset(0, 0);
    pEffect->setColor(QColor(100, 100, 100));
    pEffect->setBlurRadius(10);
    ui->frame->setGraphicsEffect(pEffect);
    ui->main_layout->setMargin(10);

    initWidgets();
    initConnections();
}

ConfigDialog::~ConfigDialog()
{
    delete ui;
}

void ConfigDialog::initWidgets()
{
    initTitleBar();

    if (qApp->property("lang") == "zh")
    {
        this->setFixedWidth(1120);
        ui->tabs->setFixedWidth(140);
    }
    else
    {
        this->setFixedWidth(1160);
        ui->tabs->setFixedWidth(180);
    }
    ui->tabs_layout->setAlignment(Qt::AlignTop);
    QHBoxLayout *camera_tab_layout = new QHBoxLayout();
    camera_tab_layout->setContentsMargins(20, 0, 20, 0);
    camera_tab_layout->setSpacing(10);
    ui->camera_tab->setLayout(camera_tab_layout);
    QLabel *camera_tab_icon = new QLabel();
    m_camera_tab_text = new QLabel();
    m_camera_tab_text->setText(tr("Camera Setting"));
    m_camera_tab_text->setStyleSheet("color: rgba(0, 0, 0, 0.80); font-size: 14px;");
    camera_tab_icon->setFixedWidth(20);
    camera_tab_icon->setFixedHeight(20);
    camera_tab_icon->setStyleSheet("background-image:url(\":/uiframe/images/config/camera_icon.svg\")");
    camera_tab_layout->addWidget(camera_tab_icon);
    camera_tab_layout->addWidget(m_camera_tab_text);

    QHBoxLayout *model_tab_layout = new QHBoxLayout();
    model_tab_layout->setContentsMargins(20, 0, 20, 0);
    model_tab_layout->setSpacing(10);
    ui->model_tab->setLayout(model_tab_layout);
    QLabel *model_tab_icon = new QLabel();
    m_model_tab_text = new QLabel();
    m_model_tab_text->setText(tr("Model Setting"));
    m_model_tab_text->setStyleSheet("color: rgba(0, 0, 0, 0.80); font-size: 14px;");
    model_tab_icon->setFixedWidth(20);
    model_tab_icon->setFixedHeight(20);
    model_tab_icon->setStyleSheet("background-image:url(\":/uiframe/images/config/model_icon.svg\")");
    model_tab_layout->addWidget(model_tab_icon);
    model_tab_layout->addWidget(m_model_tab_text);

    QHBoxLayout *system_tab_layout = new QHBoxLayout();
    system_tab_layout->setContentsMargins(20, 0, 20, 0);
    system_tab_layout->setSpacing(10);
    ui->system_tab->setLayout(system_tab_layout);
    QLabel *system_tab_icon = new QLabel();
    m_system_tab_text = new QLabel();
    m_system_tab_text->setText(tr("System Setting"));
    m_system_tab_text->setStyleSheet("color: rgba(0, 0, 0, 0.80); font-size: 14px;");
    system_tab_icon->setFixedWidth(20);
    system_tab_icon->setFixedHeight(20);
    system_tab_icon->setStyleSheet("background-image:url(\":/uiframe/images/config/sys_icon.svg\")");
    system_tab_layout->addWidget(system_tab_icon);
    system_tab_layout->addWidget(m_system_tab_text);

    QHBoxLayout *image_source_tab_layout = new QHBoxLayout();
    image_source_tab_layout->setContentsMargins(20, 0, 20, 0);
    image_source_tab_layout->setSpacing(10);
    ui->image_source_tab->setLayout(image_source_tab_layout);
    QLabel *image_source_tab_icon = new QLabel();
    m_image_source_tab_text = new QLabel();
    m_image_source_tab_text->setText(tr("Source Setting"));
    m_image_source_tab_text->setStyleSheet("color: rgba(0, 0, 0, 0.80); font-size: 14px;");
    image_source_tab_icon->setFixedWidth(20);
    image_source_tab_icon->setFixedHeight(20);

    image_source_tab_icon->setStyleSheet("background-image:url(:/uiframe/images/config/image_source.svg)");
    image_source_tab_layout->addWidget(image_source_tab_icon);
    image_source_tab_layout->addWidget(m_image_source_tab_text);

    QHBoxLayout *view_tab_layout = new QHBoxLayout();
    view_tab_layout->setContentsMargins(20, 0, 20, 0);
    view_tab_layout->setSpacing(10);
    ui->view_tab->setLayout(view_tab_layout);
    QLabel *view_tab_icon = new QLabel();
    m_view_tab_text = new QLabel();
    m_view_tab_text->setText(tr("View Setting"));
    m_view_tab_text->setStyleSheet("color: rgba(0, 0, 0, 0.80); font-size: 14px;");
    view_tab_icon->setFixedWidth(20);
    view_tab_icon->setFixedHeight(20);
    view_tab_icon->setStyleSheet("background-image:url(\":/uiframe/images/config/view_icon.svg\")");
    view_tab_layout->addWidget(view_tab_icon);
    view_tab_layout->addWidget(m_view_tab_text);

    QHBoxLayout *save_tab_layout = new QHBoxLayout();
    save_tab_layout->setContentsMargins(20, 0, 20, 0);
    save_tab_layout->setSpacing(10);
    ui->save_tab->setLayout(save_tab_layout);
    QLabel *save_tab_icon = new QLabel();
    m_save_tab_text = new QLabel();
    m_save_tab_text->setText(tr("Save Setting"));
    m_save_tab_text->setStyleSheet("color: rgba(0, 0, 0, 0.80); font-size: 14px;");
    save_tab_icon->setFixedWidth(20);
    save_tab_icon->setFixedHeight(20);
    save_tab_icon->setStyleSheet("background-image:url(\":/uiframe/images/config/save_icon.svg\")");
    save_tab_layout->addWidget(save_tab_icon);
    save_tab_layout->addWidget(m_save_tab_text);

    // add system setting page
    m_system_setting_page = new SystemSettingPage(this);
    ui->stack->addWidget(m_system_setting_page);

    // add camera setting page
    m_camera_setting_page = new CameraSettingPage(this);
    ui->stack->addWidget(m_camera_setting_page);

    // add model setting page
    m_model_setting_page = new ModelSettingPage(this);
    ui->stack->addWidget(m_model_setting_page);

    // add image source page
    m_image_source_page = new ImageSourcePage(this);
    ui->stack->addWidget(m_image_source_page);

    // add view setting page
    m_view_setting_page = new ViewSettingPage(this);
    ui->stack->addWidget(m_view_setting_page);

    // add save setting page
    m_save_setting_page = new SaveSettingPage(this);
    ui->stack->addWidget(m_save_setting_page);

    m_system_setting_page->initSystemConfigData();
    m_camera_setting_page->initCameraListWidget();
    m_model_setting_page->initModelConfigData();
    m_image_source_page->initImageSourceConfigData();
    m_view_setting_page->initViewConfigData();
    m_save_setting_page->initImageSaveConfigData();

    switchPage(0);
}

void ConfigDialog::initConnections()
{
    connect(ui->system_tab, &QPushButton::clicked, this, [=] { switchPage(0); });
    connect(ui->camera_tab, &QPushButton::clicked, this, [=] { switchPage(1); });
    connect(ui->model_tab, &QPushButton::clicked, this, [=] { switchPage(2); });
    connect(ui->image_source_tab, &QPushButton::clicked, this, [=] { switchPage(3); });
    connect(ui->view_tab, &QPushButton::clicked, this, [=] { switchPage(4); });
    connect(ui->save_tab, &QPushButton::clicked, this, [=] { switchPage(5); });
    connect(ui->save_btn, &QPushButton::clicked, this, [=] {
        ui->save_btn->setEnabled(false);
        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
        QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
        WaitingDialog waiting_dialog;
        connect(
            &waiting_dialog, &WaitingDialog::signalShowFinished, this,
            [=] {
                saveConfig();
                emit signalCloseWaitingDialog();
            },
            Qt::QueuedConnection);
        connect(this, SIGNAL(signalCloseWaitingDialog()), &waiting_dialog, SLOT(slotClose()), Qt::QueuedConnection);
        waiting_dialog.exec();
        QApplication::restoreOverrideCursor();
        ui->save_btn->setEnabled(true);
    });
    connect(ui->cancel_btn, &QPushButton::clicked, this, &ConfigDialog::cancelConfig);
    installEventFilter(this);
}

void ConfigDialog::initTitleBar()
{
    m_titleBar = new ConfigTitleBar(this);
    ui->titlebar_layout->addWidget(m_titleBar);
    connect(m_titleBar, SIGNAL(signalClose()), this, SLOT(slotCheckAndClose()));
}

void ConfigDialog::switchPage(int index)
{
    // first validate current page
    int original_index = ui->stack->currentIndex();
    if (!validatePage(original_index))
        return;
    QString selected_style = "background: white; border: 0px;";
    QString unselected_style = "background: #e9eef2; border: 0px;";
    if (original_index != index)
    {
        if (original_index == 0)
            m_system_setting_page->saveSystemConfigData();
        else if (original_index == 1)
            m_camera_setting_page->saveCameraConfigData();
        else if (original_index == 2)
            m_model_setting_page->saveModelConfigData();
        else if (original_index == 3)
            m_image_source_page->saveImageSourceConfigData();
        else if (original_index == 4)
            m_view_setting_page->saveViewConfigData();
        else
            m_save_setting_page->saveImageSaveConfigData();
    }
    ui->stack->setCurrentIndex(index);
    if (index == 0)
    {
        ui->system_tab->setStyleSheet(selected_style);
        ui->camera_tab->setStyleSheet(unselected_style);
        ui->model_tab->setStyleSheet(unselected_style);
        ui->image_source_tab->setStyleSheet(unselected_style);
        ui->view_tab->setStyleSheet(unselected_style);
        ui->save_tab->setStyleSheet(unselected_style);
    }
    else if (index == 1)
    {
        ui->system_tab->setStyleSheet(unselected_style);
        ui->camera_tab->setStyleSheet(selected_style);
        ui->model_tab->setStyleSheet(unselected_style);
        ui->image_source_tab->setStyleSheet(unselected_style);
        ui->view_tab->setStyleSheet(unselected_style);
        ui->save_tab->setStyleSheet(unselected_style);
    }
    else if (index == 2)
    {
        ui->system_tab->setStyleSheet(unselected_style);
        ui->camera_tab->setStyleSheet(unselected_style);
        ui->model_tab->setStyleSheet(selected_style);
        ui->image_source_tab->setStyleSheet(unselected_style);
        ui->view_tab->setStyleSheet(unselected_style);
        ui->save_tab->setStyleSheet(unselected_style);
    }
    else if (index == 3)
    {
        ui->system_tab->setStyleSheet(unselected_style);
        ui->camera_tab->setStyleSheet(unselected_style);
        ui->model_tab->setStyleSheet(unselected_style);
        ui->image_source_tab->setStyleSheet(selected_style);
        ui->view_tab->setStyleSheet(unselected_style);
        ui->save_tab->setStyleSheet(unselected_style);
    }
    else if (index == 4)
    {
        ui->system_tab->setStyleSheet(unselected_style);
        ui->camera_tab->setStyleSheet(unselected_style);
        ui->model_tab->setStyleSheet(unselected_style);
        ui->image_source_tab->setStyleSheet(unselected_style);
        ui->view_tab->setStyleSheet(selected_style);
        ui->save_tab->setStyleSheet(unselected_style);
    }
    else if (index == 5)
    {
        ui->system_tab->setStyleSheet(unselected_style);
        ui->camera_tab->setStyleSheet(unselected_style);
        ui->model_tab->setStyleSheet(unselected_style);
        ui->image_source_tab->setStyleSheet(unselected_style);
        ui->view_tab->setStyleSheet(unselected_style);
        ui->save_tab->setStyleSheet(selected_style);
    }
}

bool ConfigDialog::validatePage(int index)
{
    if (index == 0)
        return m_system_setting_page->validateSystemName();
    else if (index == 1)
        return m_camera_setting_page->validateCameraSettings();
    else if (index == 2)
        return m_model_setting_page->validateModelSettings();
    else if (index == 3)
        return m_image_source_page->validateImageSourceSettings();
    else if (index == 4)
        return m_view_setting_page->validateViewSettings();
    else
        return m_save_setting_page->validateSaveSettings();
}

void ConfigDialog::slotCheckAndClose()
{
    vtk::config::ConfigManager::loadFromFile();
    accept();
}

void ConfigDialog::saveConfig()
{
    const auto current_index = ui->stack->currentIndex();
    if (!validatePage(current_index))
    {
        ToastWidget::showErrorTip(tr("Save failed: You have invalid input."), this);
        return;
    }

    m_system_setting_page->saveSystemConfigData();
    m_camera_setting_page->saveCameraConfigData();
    m_model_setting_page->saveModelConfigData();
    m_image_source_page->saveImageSourceConfigData();
    m_view_setting_page->saveViewConfigData();
    m_save_setting_page->saveImageSaveConfigData();
    vtk::config::ConfigManager::saveToFile();
    ToastWidget::showTip(tr("Save the setting successfully"), this);
}

void ConfigDialog::cancelConfig()
{
    vtk::config::ConfigManager::loadFromFile();
    m_system_setting_page->cancelSystemConfig();
    m_camera_setting_page->cancelCameraConfig();
    m_model_setting_page->cancelModelConfig();
    m_image_source_page->cancelImageSourceConfig();
    m_view_setting_page->cancelViewConfig();
    m_save_setting_page->cancelImageSaveConfig();
    ToastWidget::showTip(tr("Cancel the setting successfully"), this);
}

bool ConfigDialog::eventFilter(QObject *wcg, QEvent *event)
{
    // 鼠标点击其他区域时，输入框失去焦点
    if (wcg == this && event->type() == QEvent::MouseButtonPress)
    {
        if (ui->stack->currentIndex() == 0)
            m_system_setting_page->disableFocus();
        else if (ui->stack->currentIndex() == 1)
            m_camera_setting_page->disableFocus();
        else if (ui->stack->currentIndex() == 2)
            m_model_setting_page->disableFocus();
        else if (ui->stack->currentIndex() == 3)
            m_image_source_page->disableFocus();
        else if (ui->stack->currentIndex() == 4)
            m_view_setting_page->disableFocus();
        else if (ui->stack->currentIndex() == 5)
            m_save_setting_page->disableFocus();
    }

    return QWidget::eventFilter(wcg, event);
}
}  // namespace vtk::display
