/****************************************************************************
 *  @file     errordialog.cpp
 *  @brief    主界面相机断开连接后显示的错误信息窗口类
 *  @author   ZHU Leyan
 *  @email
 *  @version
 *  @date     2021.03.05
 *  Copyright (c) 2020 SmartMore All rights reserved.
 ****************************************************************************/
#include "errordialog.h"
#include "ui_errordialog.h"

namespace vtk::display
{
ErrorDialog::ErrorDialog(QWidget *parent) : QWidget(parent), ui(new Ui::ErrorDialog)
{
    ui->setupUi(this);
}

ErrorDialog::~ErrorDialog()
{
    delete ui;
}

void ErrorDialog::on_close_btn_clicked()
{
    resetDisconnectSecs();
    this->hide();
    emit signalCloseErrorDialog();
}

void ErrorDialog::updateDisconnectSecs()
{
    m_secs += 1;
    ui->disconnect_secs->setText(QString::number(m_secs) + "s");
}

void ErrorDialog::resetDisconnectSecs()
{
    m_secs = 0;
    ui->disconnect_secs->setText("0s");
}

void ErrorDialog::changeEvent(QEvent *e)  //重写的事件处理方法
{
    QWidget::changeEvent(e);  //让基类执行事件处理方法
    switch (e->type())
    {
        case QEvent::LanguageChange:  //如果是语言改变事件
            if (ui)
                ui->retranslateUi(this);  //更新UI的语言
            break;
        default:
            break;
    }
}
}  // namespace vtk::display
