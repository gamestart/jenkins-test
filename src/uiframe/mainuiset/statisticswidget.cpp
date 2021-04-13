/****************************************************************************
 *  @file     statisticswidget.cpp
 *  @brief    统计显示窗口类
 *  @author   junjie.zeng
 *  @email    junjie.zeng@smartmore.com
 *  @version
 *  @date     2020.12.21
 *  Copyright (c) 2020 SmartMore All rights reserved.
 ****************************************************************************/
#include "statisticswidget.h"
#include "runtimecontroller/runtimecontroller.h"
#include "ui_statisticswidget.h"

namespace vtk::display
{
using namespace vtk::control;

StatisticsWidget::StatisticsWidget(QWidget *parent) : QWidget(parent), ui(new Ui::StatisticsWidget)
{
    init();
}

StatisticsWidget::~StatisticsWidget()
{
    delete ui;
}

void StatisticsWidget::on_reset_btn_clicked()
{
    m_total_counts = 0;
    m_ng_counts = 0;
    m_ok_counts = 0;
    m_pass_percent = 0.0000f;
    RuntimeController::getInstance().getStatistics().reset();
    updateStatisticsResult();
}

void StatisticsWidget::init()
{
    initWidgets();
    initConnects();
}

void StatisticsWidget::initWidgets()
{
    ui->setupUi(this);
    updateStatisticsResult();
}

void StatisticsWidget::initConnects()
{
    connect(&RuntimeController::getInstance(), &RuntimeController::signalOnStatisticsChanged, this,
            [&](const Statistics &statistics) {
                m_ng_counts = statistics.ngCounts();
                m_ok_counts = statistics.okCounts();
                m_total_counts = statistics.totalCounts();
                m_pass_percent = statistics.passPercent();
                updateStatisticsResult();
            });
}

void StatisticsWidget::updateStatisticsResult()
{
    ui->total_counts_lab->setText(QString::number(m_total_counts));
    ui->pass_counts_lab->setText(QString::number(m_ok_counts));
    ui->fail_counts_lab->setText(QString::number(m_ng_counts));
    ui->percent_lab->setText(QString::number(m_pass_percent, 'f', 2) + "%");
}

void StatisticsWidget::changeEvent(QEvent *e)  //重写的事件处理方法
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
