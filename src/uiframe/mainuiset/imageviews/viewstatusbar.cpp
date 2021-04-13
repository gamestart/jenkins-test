/****************************************************************************
 *  @file     viewstatusbar.cpp
 *  @brief    图像视窗状态条类
 *  @author   junjie.zeng
 *  @email    junjie.zeng@smartmore.com
 *  @version
 *  @date     2020.12.21
 *  Copyright (c) 2020 SmartMore All rights reserved.
 ****************************************************************************/
#include "viewstatusbar.h"
#include "ui_viewstatusbar.h"
#include <QStyle>

namespace vtk::display
{

ViewStatusBar::ViewStatusBar(QWidget *parent) : QWidget(parent), ui(new Ui::ViewStatusBar)
{
    init();
}

ViewStatusBar::~ViewStatusBar()
{
    delete ui;
}

void ViewStatusBar::init()
{
    initWidgets();
    initConnects();
}

void ViewStatusBar::initWidgets()
{
    ui->setupUi(this);
    updateResult(m_result);
}

void ViewStatusBar::initConnects()
{
    connect(ui->left_btn, &QToolButton::clicked, this, [=] {
        int current_index = ui->current_result_index->text().toInt();
        ui->current_result_index->setText(QString::number(current_index - 1));
        ui->right_btn->setEnabled(true);
        if (current_index - 1 == 1)
            ui->left_btn->setEnabled(false);
        ui->result_lab->setText(m_result.result_labs[current_index - 2]);
        emit signalUpdateImageDisplay(m_result.original_image, m_result.ai_images[current_index - 2]);
    });
    connect(ui->right_btn, &QToolButton::clicked, this, [=] {
        int current_index = ui->current_result_index->text().toInt();
        ui->current_result_index->setText(QString::number(current_index + 1));
        ui->left_btn->setEnabled(true);
        if (current_index + 1 == ui->total_result_count->text().toInt())
            ui->right_btn->setEnabled(false);
        ui->result_lab->setText(m_result.result_labs[current_index]);
        emit signalUpdateImageDisplay(m_result.original_image, m_result.ai_images[current_index]);
    });
}

void ViewStatusBar::updateResult(const ResultStatus &result)
{
    m_result = result;
    ui->total_time_lab->setText(QString::number(m_result.total_times) + "ms");
    ui->infer_lab->setText(QString::number(m_result.infer_times) + "ms");
    if (m_result.result_labs.size() && !m_result.original_image.empty())
    {
        ui->result_lab->setText(m_result.result_labs[0]);
        ui->current_result_index->setText("1");
        ui->total_result_count->setText(QString::number(m_result.result_labs.size()));
        if (m_result.result_labs.size() > 1)
            ui->right_btn->setEnabled(true);
        else
            ui->right_btn->setEnabled(false);
    }
    else
    {
        ui->current_result_index->setText("0");
        ui->total_result_count->setText("0");
        ui->result_lab->setText("");
        ui->right_btn->setEnabled(false);
    }
    ui->left_btn->setEnabled(false);

    switch (m_result.type)
    {
        case ResultStatusType::ok:
            ui->judge_status_lab->setProperty("result_type", "ok");
            break;
        case ResultStatusType::ng:
            ui->judge_status_lab->setProperty("result_type", "ng");
            break;
        default:
            ui->judge_status_lab->setProperty("result_type", "notAvailable");
            break;
    }
    style()->polish(ui->judge_status_lab);
}

void ViewStatusBar::changeEvent(QEvent *e)  //重写的事件处理方法
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
