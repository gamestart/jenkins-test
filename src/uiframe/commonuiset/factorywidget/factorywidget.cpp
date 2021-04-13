/****************************************************************************
 *  @file     factorywidget.cpp
 *  @brief    小部件添加工厂类
 *  @author   junjie.zeng
 *  @email    junjie.zeng@smartmore.com
 *  @version
 *  @date     2021.01.08
 *  Copyright (c) 2021 SmartMore All rights reserved.
 ****************************************************************************/
#include "factorywidget.h"
#include "ui_factorywidget.h"
#include <QListWidgetItem>

namespace vtk::display
{

FactoryWidget::FactoryWidget(QWidget *parent) : QWidget(parent), ui(new Ui::FactoryWidget)
{
    ui->setupUi(this);
}

FactoryWidget::~FactoryWidget()
{
    delete ui;
}

void FactoryWidget::addListItem(QListWidgetItem *item, QWidget *widget)
{
    ui->list_widget->addItem(item);
    ui->list_widget->setItemWidget(item, widget);
}

void FactoryWidget::clear()
{
    ui->list_widget->clear();
}

void FactoryWidget::deleteListItem(QListWidgetItem *item)
{
    if (item)
    {
        auto widget = ui->list_widget->itemWidget(item);
        ui->list_widget->removeItemWidget(item);
        delete widget;
        widget = nullptr;
        ui->list_widget->takeItem(ui->list_widget->row(item));
        delete item;
        item = nullptr;
    }
}

int FactoryWidget::getItemRow(QListWidgetItem *item) const
{
    return ui->list_widget->row(item);
}

int FactoryWidget::getListItemCounts() const
{
    return ui->list_widget->count();
}

QWidget *FactoryWidget::getItemWidget(int row) const
{
    return ui->list_widget->itemWidget(ui->list_widget->item(row));
}

void FactoryWidget::setEnabledAddButton(bool enable)
{
    ui->add_btn->setEnabled(enable);
}

void FactoryWidget::setAddButtonStatus(bool enable)
{
    if (enable)
    {
        ui->add_btn->setProperty("status", "enabled");
        ui->add_btn->setCursor(Qt::PointingHandCursor);
    }
    else
    {
        ui->add_btn->setProperty("status", "disabled");
        ui->add_btn->setCursor(Qt::ArrowCursor);
    }
}

QString FactoryWidget::getAddButtonStatus()
{
    return ui->add_btn->property("status").toString();
}

void FactoryWidget::on_add_btn_clicked()
{
    emit signalAddOne();
}

void FactoryWidget::setAddButtonToolTip(QString tooltip)
{
    ui->add_btn->setToolTip(tooltip);
}
}  // namespace vtk::display
