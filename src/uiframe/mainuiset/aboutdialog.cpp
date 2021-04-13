/****************************************************************************
 *  @file     aboutdialog.cpp
 *  @brief    关于页面类
 *  @author   junjie.zeng
 *  @email    junjie.zeng@smartmore.com
 *  @version
 *  @date     2020.12.21
 *  Copyright (c) 2020 SmartMore All rights reserved.
 ****************************************************************************/
#include "aboutdialog.h"
#include "ui_aboutdialog.h"
#include <QDesktopServices>
#include <QUrl>

namespace vtk::display
{

AboutDialog::AboutDialog(QWidget *parent) : QDialog(parent), ui(new Ui::AboutDialog)
{
    init();
}

AboutDialog::~AboutDialog()
{
    delete ui;
}

void AboutDialog::init()
{
    initWidgets();
}

void AboutDialog::initWidgets()
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint);
}

void AboutDialog::on_close_btn_clicked()
{
    accept();
}

void AboutDialog::on_website_btn_clicked()
{
    QDesktopServices::openUrl(QUrl("https://smartmore.com/"));
}

}  // namespace vtk::display
