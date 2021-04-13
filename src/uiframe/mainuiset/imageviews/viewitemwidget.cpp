/****************************************************************************
 *  @file     viewitemwidget.cpp
 *  @brief    缩略图像item部件类
 *  @author   junjie.zeng
 *  @email    junjie.zeng@smartmore.com
 *  @version
 *  @date     2020.12.24
 *  Copyright (c) 2020 SmartMore All rights reserved.
 ****************************************************************************/
#include "viewitemwidget.h"
#include "ui_viewitemwidget.h"

namespace vtk::display
{

ViewItemWidget::ViewItemWidget(const ImageView::ViewInfo &view_info, QWidget *parent)
    : QWidget(parent), ui(new Ui::ViewItemWidget), m_view_info(view_info)
{
    init();
}

ViewItemWidget::~ViewItemWidget()
{
    delete ui;
}

void ViewItemWidget::setThumbnail(const QImage &image)
{
    QImage scaled_img = image.scaled(ui->small_image_lab->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ui->small_image_lab->setPixmap(QPixmap::fromImage(scaled_img));
}

void ViewItemWidget::init()
{
    initWidgets();
}

void ViewItemWidget::initWidgets()
{
    ui->setupUi(this);
    ui->item_title_lab->setText(m_view_info.view_name);
}

void ViewItemWidget::initConnects() {}

}  // namespace vtk::display
