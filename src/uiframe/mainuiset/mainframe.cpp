/****************************************************************************
 *  @file     mainframe.cpp
 *  @brief    界面主显示框类
 *  @author   junjie.zeng
 *  @email    junjie.zeng@smartmore.com
 *  @version
 *  @date     2020.12.21
 *  Copyright (c) 2020 SmartMore All rights reserved.
 ****************************************************************************/
#include "mainframe.h"
#include "cameraservice/cameramanager.h"
#include "common/vtkcommon.h"
#include "configdatamanager/configmanager.h"
#include "datalistwidget.h"
#include "imageviews/doubleviewswidget.h"
#include "imageviews/errordialog.h"
#include "imageviews/fourviewswidget.h"
#include "imageviews/multiviewswidget.h"
#include "imageviews/singleviewwidget.h"
#include "imageviews/threeviewswidget.h"
#include "logwidget.h"
#include "operationalcontrolwidget.h"
#include "statisticswidget.h"
#include "titlebarwidget.h"
#include "ui_mainframe.h"
#include "uiframe/commonuiset/messagedialog/messagedialog.h"

#include <QFontDatabase>
#include <QTimer>

namespace vtk::display
{
using Message = vtk::common::Message;
using ConfigManager = vtk::config::ConfigManager;
using CameraManager = vtk::cameraservice::CameraManager;
using namespace vtk::common;

MainFrame::MainFrame(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainFrame)
{
    init();
}

MainFrame::~MainFrame()
{
    delete ui;
}

void MainFrame::init()
{
    initWidgets();
    initConnects();
}

void MainFrame::initWidgets()
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint);
    const auto &system_config_data = ConfigManager::getInstance().getSystemConfigData();
    m_titlebar_widget = new TitleBarWidget(system_config_data.app_name, "", "", this);
    m_operational_control_widget = new OperationalControlWidget;
    m_statistics_widget = new StatisticsWidget;
    m_data_list_widget = new DataListWidget(tr("Data list"));
    m_log_widget = new LogWidget(tr("Log Analysis"));

    ui->titlebar_layout->addWidget(m_titlebar_widget);
    ui->others_layout->addWidget(m_operational_control_widget);
    ui->others_layout->addWidget(m_statistics_widget);
    ui->others_layout->addWidget(m_data_list_widget);
    ui->others_layout->addWidget(m_log_widget);
    updateViewFrame();
    updateDataListWidget();
    //    m_error_dialog = new ErrorDialog;
    //    ui->view_layout->addWidget(m_error_dialog, 0, 0, 1, 1, Qt::AlignCenter);
    //    m_error_dialog->hide();
    m_timer = new QTimer;
}

void MainFrame::initConnects()
{
    connect(&Message::getInstance(), &Message::signalPostMessage, this,
            [](const QString &msg, Message::MessageType type) {
                switch (type)
                {
                    case Message::MessageType::info:
                    {
                        MessageDialog info_dialog(DialogType::InfoDialog, msg);
                        info_dialog.exec();
                        break;
                    }
                    case Message::MessageType::warning:
                    {
                        MessageDialog warning_dialog(DialogType::WarningDialog, msg);
                        warning_dialog.exec();
                        break;
                    }
                    case Message::MessageType::error:
                    {
                        MessageDialog error_dialog(DialogType::ErrorDialog, msg);
                        error_dialog.exec();
                        break;
                    }
                }
            });
    connect(&ConfigManager::getInstance(), &ConfigManager::signalConfigChanged, this, [&] {
        updateViewFrame();
        updateDataListWidget();
    });
    connect(m_operational_control_widget, &OperationalControlWidget::signalRunStatusChange, m_titlebar_widget,
            &TitleBarWidget::slotRunStatusChange);
    connect(m_titlebar_widget, SIGNAL(signalChangeLang()), this, SLOT(slotChangeLang()));
    //    connect(&CameraManager::getInstance(), &CameraManager::signalCameraStatusChange, this,
    //            [=](const std::string &camera_id, CameraState state) {
    //                switch (state)
    //                {
    //                    case CameraState::disconnect:
    //                        if (m_error_dialog->isHidden())
    //                        {
    //                            m_error_dialog->show();
    //                            m_timer->start(1000);
    //                        }
    //                        break;
    //                    default:
    //                        break;
    //                }
    //            });
    //    connect(m_timer, &QTimer::timeout, this, [=] { m_error_dialog->updateDisconnectSecs(); });
    //    connect(m_error_dialog, &ErrorDialog::signalCloseErrorDialog, this, [=] { m_timer->stop(); });
}

void MainFrame::resetViewFrame()
{
    if (m_view_frame)
    {
        if (ui->view_frame_layout->widget() == m_view_frame)
            ui->view_frame_layout->removeWidget(m_view_frame);
        delete m_view_frame;
        m_view_frame = nullptr;
    }
    std::vector<ImageView::ViewInfo> info_list;
    auto view_config_data = ConfigManager::getInstance().getViewConfigData();
    foreach (const auto &data, view_config_data)
    {
        ImageView::ViewInfo info{data.uuid, data.name};
        info_list.emplace_back(info);
    }

    switch (m_view_frame_type)
    {
        case ViewFrameType::singleView:
        {
            m_view_frame = new SingleViewWidget(info_list);
            break;
        }
        case ViewFrameType::doubleViews:
        {
            m_view_frame = new DoubleViewsWidget(info_list);
            break;
        }
        case ViewFrameType::threeViews:
        {
            m_view_frame = new ThreeViewsWidget(info_list);
            break;
        }
        case ViewFrameType::fourViews:
        {
            m_view_frame = new FourViewsWidget(info_list);
            break;
        }
        case ViewFrameType::multiViews:
        {
            m_view_frame = new MultiViewsWidget(info_list);
            break;
        }
    }
    ui->view_frame_layout->addWidget(m_view_frame);
}

void MainFrame::updateViewFrame()
{
    updateViewFrameType();
    resetViewFrame();
}

void MainFrame::updateViewFrameType()
{
    auto view_config_data = ConfigManager::getInstance().getViewConfigData();
    auto size = view_config_data.size();
    switch (size)
    {
        case 1:
            m_view_frame_type = ViewFrameType::singleView;
            break;
        case 2:
            m_view_frame_type = ViewFrameType::doubleViews;
            break;
        case 3:
            m_view_frame_type = ViewFrameType::threeViews;
            break;
        case 4:
            m_view_frame_type = ViewFrameType::fourViews;
            break;
        default:
            m_view_frame_type = ViewFrameType::multiViews;
            break;
    }
}

void MainFrame::updateDataListWidget()
{
    bool flag = false;
    auto view_config_data = ConfigManager::getInstance().getViewConfigData();
    for (const auto &data : view_config_data)
    {
        if (data.source == "File")
        {
            flag = true;
            break;
        }
    }
    if (flag)
        m_data_list_widget->show();
    else
        m_data_list_widget->hide();
}

void MainFrame::setTranslator(QTranslator *translator)
{
    m_translator = translator;
}

void MainFrame::slotChangeLang()
{
    qApp->removeTranslator(m_translator);
    m_lang = 1 - m_lang;
    if (m_lang)
    {
        m_translator->load("://uiframe/translations/smore_vtk_en.qm");
        qApp->setProperty("lang", "en");
    }
    else
    {
        m_translator->load("://uiframe/translations/smore_vtk_zh-cn.qm");
        qApp->setProperty("lang", "zh");
    }
    qApp->installTranslator(m_translator);

    ui->retranslateUi(this);
    m_data_list_widget->setTitle(tr("Data list"));
    m_log_widget->setTitle(tr("Log Analysis"));
    QStringList data_list_header;
    data_list_header << tr("No.") << tr("FileName") << tr("Contents");
    m_data_list_widget->setHeader(data_list_header);
    QStringList log_list_header;
    log_list_header << tr("DateTime") << tr("Contents");
    m_log_widget->setHeader(log_list_header);
}
}  // namespace vtk::display
