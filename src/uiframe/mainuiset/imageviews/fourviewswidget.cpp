/****************************************************************************
 *  @file     fourviewswidget.cpp
 *  @brief    四图像视窗框架类
 *  @author   junjie.zeng
 *  @email    junjie.zeng@smartmore.com
 *  @version
 *  @date     2020.12.21
 *  Copyright (c) 2020 SmartMore All rights reserved.
 ****************************************************************************/
#include "fourviewswidget.h"
#include "ui_fourviewswidget.h"

namespace vtk::display
{

FourViewsWidget::FourViewsWidget(const std::vector<ImageView::ViewInfo> &info_list, QWidget *parent)
    : QWidget(parent), ui(new Ui::FourViewsWidget)
{
    ui->setupUi(this);
    ui->view1_widget->setViewInfo(info_list.at(0));
    ui->view2_widget->setViewInfo(info_list.at(1));
    ui->view3_widget->setViewInfo(info_list.at(2));
    ui->view4_widget->setViewInfo(info_list.at(3));
}

FourViewsWidget::~FourViewsWidget()
{
    delete ui;
}

}  // namespace vtk::display
