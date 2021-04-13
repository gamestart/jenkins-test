/****************************************************************************
 *  @file     imageview.cpp
 *  @brief    图像视窗类
 *  @author   junjie.zeng
 *  @email    junjie.zeng@smartmore.com
 *  @version
 *  @date     2020.12.21
 *  Copyright (c) 2020 SmartMore All rights reserved.
 ****************************************************************************/
#include "imageview.h"
#include "common/vtkcommon.h"
#include "configdatamanager/configmanager.h"
#include "runtimecontroller/runtimecontroller.h"
#include "ui_imageview.h"
#include "viewstatusbar.h"
#include "viewtoolbar.h"
#include <QKeyEvent>

namespace vtk::display
{
using RuntimeController = vtk::control::RuntimeController;
using ConfigManager = vtk::config::ConfigManager;
using AIAlgoManager = vtk::algoservice::AIAlgoManager;
using namespace vtk::common;

ImageView::ImageView(QWidget *parent) : QWidget(parent), ui(new Ui::ImageView)
{
    init();
}

ImageView::~ImageView()
{
    delete ui;
}

void ImageView::setViewInfo(const ImageView::ViewInfo &info)
{
    m_view_info = info;
    m_view_tool_bar->setViewInfo(m_view_info);
}

const ImageView::ViewInfo &ImageView::getViewInfo() const
{
    return m_view_info;
}

void ImageView::updateViewStatusDisplay(const ImageView::ViewInfo &info)
{
    IntegratedAlgoResult algo_result;
    size_t total_times{0};
    RuntimeController::getInstance().getViewResult(info.view_uuid, algo_result, total_times);
    ViewStatusBar::ResultStatus result;
    result.total_times = total_times;
    result.infer_times = algo_result.elapsed_time;
    result.ai_images = algo_result.ai_images;
    result.original_image = algo_result.image;
    result.type = static_cast<ViewStatusBar::ResultStatusType>(algo_result.result_status);
    QStringList labs;
    for (const auto &lab : algo_result.lab_names)
        labs.append(QString::fromStdString(lab));
    result.result_labs = labs;
    m_view_status_bar->updateResult(result);
}

void ImageView::updateImageDisplay(const cv::Mat &src_image, const cv::Mat &ai_image, bool revert)
{
    QImage image;
    if (!src_image.empty())
    {
        if (m_is_ai_image && !ai_image.empty())
            image = vtk::common::matToQImage(ai_image);
        else
            image = vtk::common::matToQImage(src_image);
    }

    ui->image_graphics_view->setImage(image, revert);

    m_ai_image = ai_image;
    m_src_image = src_image;
}

void ImageView::keyPressEvent(QKeyEvent *e)
{
    if (ui->image_view->hasFocus() && (e->modifiers() == Qt::ControlModifier && e->key() == Qt::Key_0))
        ui->image_graphics_view->revertImage();
}

void ImageView::init()
{
    initWidgets();
    initConnects();
}

void ImageView::initWidgets()
{
    ui->setupUi(this);
    m_view_tool_bar = new ViewToolBar;
    m_view_status_bar = new ViewStatusBar;
    ui->toolbar_layout->addWidget(m_view_tool_bar);
    ui->statusbar_layout->addWidget(m_view_status_bar);
}

void ImageView::initConnects()
{
    connect(m_view_tool_bar, &ViewToolBar::signalChangeDisplayImageType, this, [&](bool is_ai_image) {
        m_is_ai_image = is_ai_image;
        auto mat = (m_is_ai_image && !m_ai_image.empty()) ? m_ai_image : m_src_image;
        auto image = vtk::common::matToQImage(mat);
        ui->image_graphics_view->setImage(image, false);
        emit signalUpdateThumbnail(mat);
    });
    connect(m_view_tool_bar, &ViewToolBar::signalZoomoutImage, this, [&] { ui->image_graphics_view->zoomoutImage(); });
    connect(m_view_tool_bar, &ViewToolBar::signalZoominImage, this, [&] { ui->image_graphics_view->zoominImage(); });
    connect(m_view_tool_bar, &ViewToolBar::signalRevertImage, this, [&] { ui->image_graphics_view->revertImage(); });
    connect(ui->image_graphics_view, &ImageGraphicsView::signalClicked, this, [&] { ui->image_view->setFocus(); });
    connect(ui->image_graphics_view, &ImageGraphicsView::signalUpdateFactor, this,
            [&](float factor) { m_view_tool_bar->setScalefactor(factor * 100); });
    connect(&RuntimeController::getInstance(), &RuntimeController::signalOnAlgoResult, this,
            [&](const QUuid &view_uuid) {
                IntegratedAlgoResult integrated_result;
                size_t total_times{0};
                RuntimeController::getInstance().getViewResult(view_uuid, integrated_result, total_times);
                if (m_is_ai_image && integrated_result.ai_images.size() && !integrated_result.ai_images[0].empty())
                {
                    emit signalUpdateThumbnail(view_uuid, integrated_result.ai_images[0]);
                }
                else
                {
                    emit signalUpdateThumbnail(view_uuid, integrated_result.image);
                }
                if (m_view_info.view_uuid == view_uuid)
                {
                    updateImageDisplay(integrated_result.image, integrated_result.ai_images.at(0), false);
                    ViewStatusBar::ResultStatus result;
                    result.infer_times = integrated_result.elapsed_time;
                    result.total_times = result.infer_times == 0 ? 0 : total_times;
                    result.ai_images = integrated_result.ai_images;
                    result.original_image = integrated_result.image;
                    result.type = static_cast<ViewStatusBar::ResultStatusType>(integrated_result.result_status);
                    QStringList labs;
                    for (const auto &lab : integrated_result.lab_names)
                        labs.append(QString::fromStdString(lab));
                    result.result_labs = labs;
                    m_view_status_bar->updateResult(result);
                }
            });
    connect(m_view_tool_bar, &ViewToolBar::signalUpdateImage, this, [&](const cv::Mat &mat) {
        updateImageDisplay(mat, mat, true);
        ViewStatusBar::ResultStatus result;
        m_view_status_bar->updateResult(result);
        emit signalUpdateThumbnail(mat);
    });
    connect(m_view_status_bar, &ViewStatusBar::signalUpdateImageDisplay, this,
            [&](const cv::Mat &original_image, const cv::Mat &ai_image) {
                updateImageDisplay(original_image, ai_image, false);
                if (m_is_ai_image && !ai_image.empty())
                {
                    emit signalUpdateThumbnail(ai_image);
                }
                else
                {
                    emit signalUpdateThumbnail(original_image);
                }
            });
}

}  // namespace vtk::display
