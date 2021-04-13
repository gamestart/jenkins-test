/****************************************************************************
 *  @file     addmodelwidget.cpp
 *  @brief    设置窗口添加模型窗口类
 *  @author
 *  @email
 *  @version
 *  @date     2021.01.07
 *  Copyright (c) 2020 SmartMore All rights reserved.
 ****************************************************************************/
#include "addmodelwidget.h"
#include "ui_addmodelwidget.h"

namespace vtk::display
{

AddModelWidget::AddModelWidget(QWidget *parent) : QWidget(parent), ui(new Ui::AddModelWidget)
{
    ui->setupUi(this);
}

AddModelWidget::~AddModelWidget()
{
    delete ui;
}

void AddModelWidget::on_add_model_btn_clicked()
{
    emit signalAddModel();
}

}  // namespace vtk::display
