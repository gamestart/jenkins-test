/****************************************************************************
 *  @file     singleviewwidget.cpp
 *  @brief    单图像视窗框架类
 *  @author   junjie.zeng
 *  @email    junjie.zeng@smartmore.com
 *  @version
 *  @date     2020.12.21
 *  Copyright (c) 2020 SmartMore All rights reserved.
 ****************************************************************************/
#include "singleviewwidget.h"
#include "ui_singleviewwidget.h"

namespace vtk::display
{

SingleViewWidget::SingleViewWidget(const std::vector<ImageView::ViewInfo> &info_list, QWidget *parent)
    : QWidget(parent), ui(new Ui::SingleViewWidget)
{
    ui->setupUi(this);
    assert(info_list.size() == 1);
    ui->view_widget->setViewInfo(info_list.at(0));
}

SingleViewWidget::~SingleViewWidget()
{
    delete ui;
}

}  // namespace vtk::display
