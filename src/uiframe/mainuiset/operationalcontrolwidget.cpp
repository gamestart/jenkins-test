/****************************************************************************
 *  @file     operationalcontrolwidget.cpp
 *  @brief    运行控制窗口类
 *  @author   junjie.zeng
 *  @email    junjie.zeng@smartmore.com
 *  @version
 *  @date     2020.12.21
 *  Copyright (c) 2020 SmartMore All rights reserved.
 ****************************************************************************/
#include "operationalcontrolwidget.h"
#include "ui_operationalcontrolwidget.h"
#include <QStyle>

namespace vtk::display
{
using RuntimeController = vtk::control::RuntimeController;
using RunStatus = vtk::control::RunStatus;
OperationalControlWidget::OperationalControlWidget(QWidget *parent)
    : QWidget(parent), ui(new Ui::OperationalControlWidget)
{
    init();
}

OperationalControlWidget::~OperationalControlWidget()
{
    delete ui;
}

void OperationalControlWidget::on_run_once_btn_clicked()
{
    if (ui->run_once_btn->property("run_status").toString() == "stop")
        RuntimeController::getInstance().slotRunOnce();
}

void OperationalControlWidget::on_run_cycle_btn_clicked()
{
    if (ui->run_cycle_btn->property("run_status").toString() == "stop")
        RuntimeController::getInstance().slotRunCycle();
}

void OperationalControlWidget::on_pause_btn_clicked()
{
    RuntimeController::getInstance().slotRunPause();
}

void OperationalControlWidget::init()
{
    initWidgets();
    initConnects();
}

void OperationalControlWidget::initWidgets()
{
    ui->setupUi(this);
    ui->run_once_btn->setProperty("run_status", "stop");
    ui->run_cycle_btn->setProperty("run_status", "stop");
}

void OperationalControlWidget::initConnects()
{
    connect(&RuntimeController::getInstance(), &RuntimeController::signalOnRunStatusChanged, this,
            [&](RunStatus status) {
                switch (status)
                {
                    case RunStatus::Offline:
                    {
                        ui->run_once_btn->setProperty("run_status", "stop");
                        ui->run_cycle_btn->setProperty("run_status", "stop");
                        ui->pause_btn->setEnabled(false);
                        ui->run_cycle_btn->setEnabled(true);
                        ui->run_once_btn->setEnabled(true);
                        ui->run_status_lab->setText(tr("Offline"));
                        ui->run_status_lab->setEnabled(false);
                        style()->polish(ui->run_once_btn);
                        style()->polish(ui->run_cycle_btn);
                    }
                    break;
                    case RunStatus::Once:
                    {
                        ui->run_once_btn->setProperty("run_status", "run");
                        ui->pause_btn->setEnabled(true);
                        ui->run_cycle_btn->setEnabled(false);
                        ui->run_status_lab->setText(tr("RUN"));
                        ui->run_status_lab->setEnabled(true);
                        style()->polish(ui->run_once_btn);
                    }
                    break;
                    case RunStatus::Cycle:
                    {
                        ui->run_cycle_btn->setProperty("run_status", "run");
                        ui->pause_btn->setEnabled(true);
                        ui->run_once_btn->setEnabled(false);
                        ui->run_status_lab->setText(tr("RUN"));
                        ui->run_status_lab->setEnabled(true);
                        style()->polish(ui->run_cycle_btn);
                    }
                    break;
                }
                emit signalRunStatusChange(status);
            });
}

void OperationalControlWidget::changeEvent(QEvent *e)  //重写的事件处理方法
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
