/****************************************************************************
 *  @file     threeviewswidget.cpp
 *  @brief    三图像视窗框架类
 *  @author   junjie.zeng
 *  @email    junjie.zeng@smartmore.com
 *  @version
 *  @date     2020.12.21
 *  Copyright (c) 2020 SmartMore All rights reserved.
 ****************************************************************************/
#include "threeviewswidget.h"
#include "ui_threeviewswidget.h"

namespace vtk::display
{

ThreeViewsWidget::ThreeViewsWidget(const std::vector<ImageView::ViewInfo> &info_list, QWidget *parent)
    : QWidget(parent), ui(new Ui::ThreeViewsWidget)
{
    ui->setupUi(this);
    ui->view1_widget->setViewInfo(info_list.at(0));
    ui->view2_widget->setViewInfo(info_list.at(1));
    ui->view3_widget->setViewInfo(info_list.at(2));
}

ThreeViewsWidget::~ThreeViewsWidget()
{
    delete ui;
}

}  // namespace vtk::display
