/****************************************************************************
 *  @file     doubleviewswidget.cpp
 *  @brief    两图像视窗框架类
 *  @author   junjie.zeng
 *  @email    junjie.zeng@smartmore.com
 *  @version
 *  @date     2020.12.21
 *  Copyright (c) 2020 SmartMore All rights reserved.
 ****************************************************************************/
#include "doubleviewswidget.h"
#include "ui_doubleviewswidget.h"

namespace vtk::display
{

DoubleViewsWidget::DoubleViewsWidget(const std::vector<ImageView::ViewInfo> &info_list, QWidget *parent)
    : QWidget(parent), ui(new Ui::DoubleViewsWidget)
{
    ui->setupUi(this);
    assert(info_list.size() == 2);
    ui->view1_widget->setViewInfo(info_list.at(0));
    ui->view2_widget->setViewInfo(info_list.at(1));
}

DoubleViewsWidget::~DoubleViewsWidget()
{
    delete ui;
}

}  // namespace vtk::display
