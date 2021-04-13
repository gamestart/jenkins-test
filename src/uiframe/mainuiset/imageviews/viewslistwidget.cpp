/****************************************************************************
 *  @file     viewslistwidget.cpp
 *  @brief    缩略图像列表窗口部件类
 *  @author   junjie.zeng
 *  @email    junjie.zeng@smartmore.com
 *  @version
 *  @date     2020.12.21
 *  Copyright (c) 2020 SmartMore All rights reserved.
 ****************************************************************************/
#include "viewslistwidget.h"
#include "configdatamanager/configmanager.h"
#include "runtimecontroller/runtimecontroller.h"
#include "ui_viewslistwidget.h"

namespace vtk::display
{
using RuntimeController = vtk::control::RuntimeController;
using ConfigManager = vtk::config::ConfigManager;
using namespace vtk::common;

ViewsListWidget::ViewsListWidget(QWidget *parent) : QWidget(parent), ui(new Ui::ViewsListWidget)
{
    init();
}

ViewsListWidget::~ViewsListWidget()
{
    delete ui;
}

void ViewsListWidget::resetViewList(const std::vector<ImageView::ViewInfo> &view_info_list)
{
    for (int i = 0; i < ui->view_list_widget->count(); i++)
    {
        auto item = ui->view_list_widget->item(i);
        auto widget = dynamic_cast<ViewItemWidget *>(ui->view_list_widget->itemWidget(item));
        ui->view_list_widget->removeItemWidget(item);
        if (widget)
            delete widget;
    }

    ui->view_list_widget->clear();
    m_view_info_list.clear();
    m_view_info_list = view_info_list;
    for (const auto &info : m_view_info_list)
    {
        auto item = new QListWidgetItem;
        auto widget = new ViewItemWidget(info);
        item->setData(Qt::UserRole, QVariant::fromValue(info));
        item->setSizeHint(QSize(130, 130));
        ui->view_list_widget->addItem(item);
        ui->view_list_widget->setItemWidget(item, widget);
    }
    ui->view_list_widget->setCurrentRow(0);
}

void ViewsListWidget::slotUpdateThumbnail(const cv::Mat &mat)
{
    const auto item = ui->view_list_widget->currentItem();
    if (item)
    {
        const auto &info = item->data(Qt::UserRole).value<ImageView::ViewInfo>();
        if (!m_view_uuid_to_src_image_map.keys().contains(info.view_uuid))
        {
            m_view_uuid_to_src_image_map[info.view_uuid] = mat;
            m_view_uuid_to_ai_images_map[info.view_uuid] = {mat};
        }
        auto widget = dynamic_cast<ViewItemWidget *>(ui->view_list_widget->itemWidget(item));
        if (widget)
        {
            QImage image;
            image = vtk::common::matToQImage(mat);
            widget->setThumbnail(image);
        }
    }
}

void ViewsListWidget::slotUpdateThumbnail(const QUuid &uuid, const cv::Mat &mat)
{
    for (int i = 0; i < ui->view_list_widget->count(); i++)
    {
        auto item = ui->view_list_widget->item(i);
        if (item)
        {
            const auto &info = item->data(Qt::UserRole).value<ImageView::ViewInfo>();
            if (info.view_uuid == uuid)
            {
                if (!m_view_uuid_to_src_image_map.keys().contains(info.view_uuid))
                {
                    m_view_uuid_to_src_image_map[info.view_uuid] = mat;
                    m_view_uuid_to_ai_images_map[info.view_uuid] = {mat};
                }
                auto widget = dynamic_cast<ViewItemWidget *>(ui->view_list_widget->itemWidget(item));
                if (widget)
                {
                    QImage image;
                    image = vtk::common::matToQImage(mat);
                    widget->setThumbnail(image);
                }
            }
        }
    }
}

void ViewsListWidget::init()
{
    initWidgets();
    initConnects();
}

void ViewsListWidget::initWidgets()
{
    ui->setupUi(this);
}

void ViewsListWidget::initConnects()
{
    connect(ui->view_list_widget, &QListWidget::currentRowChanged, this, [&](int row) {
        auto item = ui->view_list_widget->item(row);
        auto info = item->data(Qt::UserRole).value<ImageView::ViewInfo>();
        if (m_view_uuid_to_src_image_map.find(info.view_uuid) != m_view_uuid_to_src_image_map.end() &&
            m_view_uuid_to_ai_images_map.find(info.view_uuid) != m_view_uuid_to_ai_images_map.end())
            emit signalViewChange(info, m_view_uuid_to_src_image_map[info.view_uuid],
                                  m_view_uuid_to_ai_images_map[info.view_uuid][0]);
        else if (m_view_uuid_to_src_image_map.find(info.view_uuid) != m_view_uuid_to_src_image_map.end())
            emit signalViewChange(info, m_view_uuid_to_src_image_map[info.view_uuid], cv::Mat());
        else
            emit signalViewChange(info, cv::Mat(), cv::Mat());
    });
    connect(&RuntimeController::getInstance(), &RuntimeController::signalOnAlgoResult, this,
            [&](const QUuid &view_uuid) {
                IntegratedAlgoResult integrated_result;
                size_t total_times{0};
                RuntimeController::getInstance().getViewResult(view_uuid, integrated_result, total_times);
                m_view_uuid_to_src_image_map[view_uuid] = integrated_result.image;
                m_view_uuid_to_ai_images_map[view_uuid] = integrated_result.ai_images;
            });
}

void ViewsListWidget::updateThumbnail(const QUuid &view_uuid, const cv::Mat &mat)
{
    //更新同一数源对应所有视窗的缩略图
    QList<int> view_pos_list;
    RuntimeController::getInstance().getViewsPosOfSameSource(view_uuid, view_pos_list);
    if (view_pos_list.size())
    {
        ui->view_list_widget->setCurrentRow(view_pos_list.at(0));
    }
    for (auto pos : view_pos_list)
    {
        const auto item = ui->view_list_widget->item(pos);
        if (item)
        {
            auto widget = dynamic_cast<ViewItemWidget *>(ui->view_list_widget->itemWidget(item));
            if (widget)
            {
                QImage image;
                image = vtk::common::matToQImage(mat);
                widget->setThumbnail(image);
            }
        }
    }
}

}  // namespace vtk::display
