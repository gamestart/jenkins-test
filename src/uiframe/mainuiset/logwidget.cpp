/****************************************************************************
 *  @file     logwidget.cpp
 *  @brief    日志列表类
 *  @author   junjie.zeng
 *  @email    junjie.zeng@smartmore.com
 *  @version
 *  @date     2020.12.21
 *  Copyright (c) 2020 SmartMore All rights reserved.
 ****************************************************************************/
#include "logwidget.h"
#include "logservice/logservice.h"
#include "ui_logwidget.h"
#include <QDateTime>
#include <QFileDialog>

namespace vtk::display
{

using namespace vtk::logservice;

LogWidget::LogWidget(const QString &title, QWidget *parent) : QWidget(parent), ui(new Ui::LogWidget), m_title(title)
{
    init();
}

LogWidget::~LogWidget()
{
    delete ui;
}

void LogWidget::init()
{
    initWidgets();
    initConnects();
}

void LogWidget::initWidgets()
{
    ui->setupUi(this);
    ui->log_list_widget->setTitle(m_title);
    std::vector<TableWidget::HeadData> head_data_list;
    TableWidget::HeadData head_data = {tr("Time"), 102};
    head_data_list.emplace_back(head_data);
    head_data = {tr("Contents"), 202};
    head_data_list.emplace_back(head_data);
    ui->log_list_widget->initTable(m_title, head_data_list);
    QStringList list;
    list << "All"
         << "Error"
         << "Info"
         << "Warning"
         << "AI"
         << "Camera"
         << "Others";
    ui->log_list_widget->resetCombox(list);
}

void LogWidget::initConnects()
{
    connect(&Logger::getInstance(), &Logger::signalAppendLog, this, [&](const LogItem &log_item) {
        QList<QVariant> list;
        QVariant item;
        item = log_item.ctime;
        list.append(item);

        item = log_item.content;
        list.append(item);
        ui->log_list_widget->appendRow(list);
    });
    connect(&Logger::getInstance(), &Logger::signalReloadLog, this, [&](const std::list<LogItem *> &logs) {
        ui->log_list_widget->deleteAllRows();
        QList<QList<QVariant>> lists;
        for (const auto &log_item : logs)
        {
            QList<QVariant> list;
            QVariant item;
            item = log_item->ctime;
            list.append(item);

            item = log_item->content;
            list.append(item);

            lists.append(list);
        }
        ui->log_list_widget->appendRows(lists);
    });

    connect(ui->log_list_widget, &TableWidget::signalComboxTextChange, this, [&](const QString &text) {
        if (text == "All")
        {
            LogFilter filter;
            Logger::getInstance().setFilter(filter);
        }
        else if (text == "Error")
        {
            Logger::getInstance().setFilter(LogLevel::Error);
        }
        else if (text == "Info")
        {
            Logger::getInstance().setFilter(LogLevel::Info);
        }
        else if (text == "Warning")
        {
            Logger::getInstance().setFilter(LogLevel::Warning);
        }
        else if (text == "AI")
        {
            Logger::getInstance().setFilter(LogSource::AI);
        }
        else if (text == "Camera")
        {
            Logger::getInstance().setFilter(LogSource::Camera);
        }
        else if (text == "Others")
        {
            Logger::getInstance().setFilter(LogSource::Others);
        }
    });
}

void LogWidget::setTitle(QString title)
{
    ui->log_list_widget->setTitle(title);
}

void LogWidget::setHeader(QStringList header_list)
{
    ui->log_list_widget->setHeader(header_list);
}

}  // namespace vtk::display
