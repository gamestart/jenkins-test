/****************************************************************************
 *  @file     datalistwidget.cpp
 *  @brief    数据列表类
 *  @author   junjie.zeng
 *  @email    junjie.zeng@smartmore.com
 *  @version
 *  @date     2020.12.21
 *  Copyright (c) 2020 SmartMore All rights reserved.
 ****************************************************************************/
#include "datalistwidget.h"
#include "common/vtkcommon.h"
#include "configdatamanager/configmanager.h"
#include "runtimecontroller/runtimecontroller.h"
#include "ui_datalistwidget.h"
#include <QFileInfo>
#include <QTextCodec>
#include <opencv2/opencv.hpp>

#define CHECKEDSTATUSCOL 2
#define VIEWIDCOL 3
#define RESULTSTATUSCOL 5
#define IMAGEIDCOL 6
#define NUMCOL 0

namespace vtk::display
{
using namespace vtk::common;
using RuntimeController = vtk::control::RuntimeController;
using ConfigManager = vtk::config::ConfigManager;

DataListWidget::DataListWidget(const QString &title, QWidget *parent)
    : QWidget(parent), ui(new Ui::DataListWidget), m_title(title)
{
    init();
}

DataListWidget::~DataListWidget()
{
    delete ui;
}

void DataListWidget::init()
{
    initWidgets();
    initConnects();
}

void DataListWidget::initWidgets()
{
    ui->setupUi(this);
    ui->data_list_widget->setTitle(m_title);
    std::vector<TableWidget::HeadData> head_data_list;
    TableWidget::HeadData head_data = {tr("No."), 40};
    head_data_list.emplace_back(head_data);
    head_data = {tr("FileName"), 102};
    head_data_list.emplace_back(head_data);
    head_data = {tr("Contents"), 162};
    head_data_list.emplace_back(head_data);
    head_data = {tr("ViewId"), 0};
    head_data_list.emplace_back(head_data);
    head_data = {tr("ImagePath"), 0};
    head_data_list.emplace_back(head_data);
    head_data = {tr("ResultStatus"), 0};
    head_data_list.emplace_back(head_data);
    head_data = {tr("ImageUuid"), 0};
    head_data_list.emplace_back(head_data);
    ui->data_list_widget->initTable(m_title, head_data_list);
    ui->data_list_widget->setColumnHidden(3, true);
    ui->data_list_widget->setColumnHidden(4, true);
    ui->data_list_widget->setColumnHidden(5, true);
    ui->data_list_widget->setColumnHidden(6, true);
    resetCombox();
}

void DataListWidget::initConnects()
{
    connect(ui->data_list_widget, &TableWidget::signalSendSelectRowData, this, [&](const QList<QVariant> &row_data) {
        if (row_data.count() != 7)
            return;
        auto view_uuid = row_data.at(3).toUuid();
        auto file_path = row_data.at(4).toString();
        auto image_uuid = row_data.at(6).toUuid();
#ifdef Q_OS_WIN
        QTextCodec *gbk_code = QTextCodec::codecForName("GBK");
        auto path = std::string(gbk_code->fromUnicode(file_path).data());
#else
                auto path = file_path.toStdString();
#endif
        auto input_image = cv::imread(std::string(path));
        RuntimeController::getInstance().slotRunLocalImage(view_uuid, input_image, image_uuid);
    });

    connect(&ImageListForwarder::getInstance(), &ImageListForwarder::signalPostImageListInfo, this,
            [&](const QUuid &view_uuid, const QList<ImageListForwarder::ImageInfo> &image_list) {
                if (image_list.size() == 0)
                    return;
                QList<QList<QVariant>> lists;
                auto count = ui->data_list_widget->getShownTableRowCount();
                foreach (const auto &image_info, image_list)
                {
                    QList<QVariant> list;
                    QVariant item;
                    item = ++count;
                    list.append(item);

                    item = QFileInfo(image_info.image_path).fileName();
                    list.append(item);

                    item = "";
                    list.append(item);

                    item = view_uuid;
                    list.append(item);

                    item = image_info.image_path;
                    list.append(item);

                    item = QString("notAvailable");
                    list.append(item);

                    item = image_info.image_uuid;
                    list.append(item);

                    lists.append(list);
                }

                ui->data_list_widget->appendRows(lists);
            });
    connect(ui->data_list_widget, &TableWidget::signalComboxTextChange, this, [&](const QString &text) {
        if (text == "All")
            ui->data_list_widget->setFilter();
        else if (text == "OK")
            ui->data_list_widget->setFilter(RESULTSTATUSCOL, QString("OK"));
        else if (text == "NG")
            ui->data_list_widget->setFilter(RESULTSTATUSCOL, QString("NG"));
        else
        {
            auto iter = m_view_info_hash.begin();
            while (iter != m_view_info_hash.end())
            {
                if (iter.value() == text)
                {
                    ui->data_list_widget->setFilter(VIEWIDCOL, iter.key());
                    break;
                }
                iter++;
            }
        }
        resetNum();
    });
    connect(&ConfigManager::getInstance(), &ConfigManager::signalConfigChanged, this, [&] {
        checkImageList();
        resetCombox();
    });

    connect(&RuntimeController::getInstance(), &RuntimeController::signalOnAlgoResult, this,
            [&](const QUuid &view_uuid, const vtk::common::AlgoResult &algo_result, const QUuid &image_uuid,
                size_t total_elapsed_time) {
                (void)view_uuid;
                (void)total_elapsed_time;
                if (image_uuid.isNull())
                    return;
                int row = ui->data_list_widget->getImageRow(image_uuid);
                QString labs;
                for (const auto &lab : algo_result.lab_names)
                    labs.append(QString::fromStdString(lab) + " ");
                QString tip = labs;
                QString status;
                switch (algo_result.result_status)
                {
                    case ResultStatusType::ok:
                        status = "OK";
                        break;
                    case ResultStatusType::ng:
                        status = "NG";
                        break;
                    case ResultStatusType::notAvailable:
                    default:
                        status = "NONE";
                        break;
                }
                tip += status;
                ui->data_list_widget->setData(row, CHECKEDSTATUSCOL, tip);
                ui->data_list_widget->setData(row, RESULTSTATUSCOL, status);
            });
}

void DataListWidget::resetCombox()
{
    m_view_info_hash.clear();
    QStringList list;
    list << "All"
         << "OK"
         << "NG";
    auto view_config_data = ConfigManager::getInstance().getViewConfigData();
    foreach (const auto &data, view_config_data)
    {
        if (data.source == "File")
        {
            list.append(data.name);
            m_view_info_hash[data.uuid] = data.name;
        }
    }
    ui->data_list_widget->resetCombox(list);
}

void DataListWidget::checkImageList()
{
    QList<QUuid> new_id_list;
    auto view_config_data = ConfigManager::getInstance().getViewConfigData();
    foreach (const auto &data, view_config_data)
    {
        if (data.source == "File")
        {
            new_id_list.append(data.uuid);
        }
    }

    auto iter = m_view_info_hash.begin();
    while (iter != m_view_info_hash.end())
    {
        if (!new_id_list.contains(iter.key()))
            ui->data_list_widget->deleteRowsByCondition(VIEWIDCOL, iter.key());
        iter++;
    }
    resetNum();
}

void DataListWidget::resetNum()
{
    int count = 0;
    for (int i = 0; i < ui->data_list_widget->getTableRowCount(); i++)
    {
        if (!ui->data_list_widget->isRowHidden(i))
        {
            count++;
            ui->data_list_widget->setData(i, NUMCOL, count);
        }
    }
}

void DataListWidget::setTitle(QString title)
{
    ui->data_list_widget->setTitle(title);
}

void DataListWidget::setHeader(QStringList header_list)
{
    ui->data_list_widget->setHeader(header_list);
}

}  // namespace vtk::display
