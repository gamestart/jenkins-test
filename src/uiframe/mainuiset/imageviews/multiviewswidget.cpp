/****************************************************************************
 *  @file     multiviewswidget.cpp
 *  @brief    多图像视窗框架类（大于四视窗）
 *  @author   junjie.zeng
 *  @email    junjie.zeng@smartmore.com
 *  @version
 *  @date     2020.12.21
 *  Copyright (c) 2020 SmartMore All rights reserved.
 ****************************************************************************/
#include "multiviewswidget.h"
#include "ui_multiviewswidget.h"

namespace vtk::display
{

MultiViewsWidget::MultiViewsWidget(const std::vector<ImageView::ViewInfo> &info_list, QWidget *parent)
    : QWidget(parent), ui(new Ui::MultiViewsWidget)
{
    ui->setupUi(this);
    ui->view_widget->setViewInfo(info_list.at(0));
    ui->view_list_widget->resetViewList(info_list);
    connect(ui->view_widget, SIGNAL(signalUpdateThumbnail(const cv::Mat &)), ui->view_list_widget,
            SLOT(slotUpdateThumbnail(const cv::Mat &)));
    connect(ui->view_widget, SIGNAL(signalUpdateThumbnail(const QUuid &, const cv::Mat &)), ui->view_list_widget,
            SLOT(slotUpdateThumbnail(const QUuid &, const cv::Mat &)));
    connect(ui->view_list_widget, &ViewsListWidget::signalViewChange, this,
            [&](const ImageView::ViewInfo &info, const cv::Mat &src_image, const cv::Mat &ai_image) {
                ui->view_widget->setViewInfo(info);
                ui->view_widget->updateViewStatusDisplay(info);
                ui->view_widget->updateImageDisplay(src_image, ai_image, true);
            });
}

MultiViewsWidget::~MultiViewsWidget()
{
    delete ui;
}

}  // namespace vtk::display
