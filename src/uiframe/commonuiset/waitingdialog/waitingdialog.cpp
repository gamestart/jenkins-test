/****************************************************************************
 *  @file     waitingdialog.cpp
 *  @brief    通用等待窗口类
 *  @author   ZHU Leyan
 *  @email
 *  @version
 *  @date     2021.02.05
 *  Copyright (c) 2020 SmartMore All rights reserved.
 ****************************************************************************/
#include "waitingdialog.h"
#include "ui_waitingdialog.h"

#include <QMovie>
#include <QTimer>

namespace vtk::display
{

WaitingDialog::WaitingDialog(QWidget *parent) : QDialog(parent), ui(new Ui::WaitingDialog)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
}

WaitingDialog::~WaitingDialog()
{
    delete ui;
}

void WaitingDialog::showEvent(QShowEvent *event)
{
    emit signalShowFinished();
    QDialog::showEvent(event);
}

void WaitingDialog::slotClose()
{
    accept();
}
}  // namespace vtk::display
