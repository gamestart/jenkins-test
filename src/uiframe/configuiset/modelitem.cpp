/****************************************************************************
 *  @file     modelitem.cpp
 *  @brief    设置窗口显示设置检测模型列表单个模型类
 *  @author   ZHU Leyan
 *  @email
 *  @version
 *  @date     2021.01.08
 *  Copyright (c) 2020 SmartMore All rights reserved.
 ****************************************************************************/
#include "modelitem.h"
#include "configdatamanager/configmanager.h"
#include "ui_modelitem.h"

#include <QListView>
#include <QStandardItemModel>

namespace vtk::display
{
using namespace vtk::common;
using ConfigManager = vtk::config::ConfigManager;
using AlgoConfigData = vtk::config::AlgoConfigData;

ModelItem::ModelItem(QString model_name, QWidget *parent) : QWidget(parent), ui(new Ui::ModelItem)
{
    ui->setupUi(this);
    ui->model_item_list->setView(new QListView());
    AlgoConfigData algo_data = ConfigManager::getInstance().getAlgoConfigData();
    AlgoType model_type{0};
    for (const auto &algo : algo_data.items)
    {
        if (algo.type == AlgoType::noType)
            continue;
        if (!emit signalIsNameUsed(algo.name))
        {
            QVariant var;
            var.setValue(algo.uuid);
            ui->model_item_list->addItem(algo.name, var);
            if (algo.name == model_name)
                model_type = algo.type;
        }
    }
    if (model_name.isEmpty())
        ui->model_item_list->setCurrentIndex(-1);
    else
        ui->model_item_list->setCurrentText(model_name);
    QString type = convertAlgoType(model_type);
    ui->model_item_type->setText(type);
    ui->model_item_list->installEventFilter(this);
}

ModelItem::~ModelItem()
{
    delete ui;
}

QString ModelItem::convertAlgoType(AlgoType model_type)
{
    QString type;
    switch (model_type)
    {
        case AlgoType::classificationType:
            type = tr("Classification Algorithm");
            break;
        case AlgoType::ocrType:
            type = tr("OCR Algorithm");
            break;
        case AlgoType::detectionType:
            type = tr("Detection Algorithm");
            break;
        case AlgoType::segmentationType:
            type = tr("Segmentation Algorithm");
            break;
        default:
            type = "";
            break;
    }
    return type;
}

QString ModelItem::getText()
{
    return ui->model_item_list->currentText();
}

void ModelItem::setText(QString name)
{
    if (name.isEmpty())
    {
        ui->model_item_list->setCurrentIndex(-1);
        ui->model_item_type->setText("");
        return;
    }
    ui->model_item_list->setCurrentText(name);
    setTypeText(name);
}

void ModelItem::setId(int id)
{
    ui->model_item_id->setText(QString::number(id));
}

void ModelItem::setTypeText(QString name)
{
    if (name.isEmpty())
    {
        ui->model_item_type->setText("");
        return;
    }
    AlgoConfigData algo_data = ConfigManager::getInstance().getAlgoConfigData();
    for (const auto &algo : algo_data.items)
    {
        if (algo.name == name)
            ui->model_item_type->setText(convertAlgoType(algo.type));
    }
}

QUuid ModelItem::getUuid(QString name)
{
    AlgoConfigData algo_data = ConfigManager::getInstance().getAlgoConfigData();
    for (const auto &algo : algo_data.items)
    {
        if (algo.name == name)
            return algo.uuid;
    }
    return "";
}

void ModelItem::on_model_item_list_currentTextChanged(const QString &name)
{
    setTypeText(name);
    emit signalNameSelected(ui->model_item_id->text().toInt());
}

void ModelItem::resetList()
{
    QUuid original_id = ui->model_item_list->currentData().toUuid();
    ui->model_item_list->clear();
    AlgoConfigData algo_data = ConfigManager::getInstance().getAlgoConfigData();
    for (const auto &algo : algo_data.items)
    {
        if (algo.type == AlgoType::noType)
            continue;
        QVariant var;
        var.setValue(algo.uuid);
        if (ui->model_item_list->findText(algo.name) < 0)
            ui->model_item_list->addItem(algo.name, var);
    }
    bool find = false;
    for (int i = 0; i < ui->model_item_list->count(); i++)
    {
        if (original_id == ui->model_item_list->itemData(i).toUuid())
        {
            ui->model_item_list->setCurrentIndex(i);
            find = true;
            break;
        }
    }
    if (!find)
    {
        emit signalDelete();
    }
}

void ModelItem::changeItemStatus()
{
    QUuid original_id = ui->model_item_list->currentData().toUuid();
    QString original_text = ui->model_item_list->currentText();
    for (int i = 0; i < ui->model_item_list->count(); i++)
    {
        if (original_id == ui->model_item_list->itemData(i).toUuid())
        {
            continue;
        }
        if (emit signalIsNameUsed(ui->model_item_list->itemText(i)))
        {
            ui->model_item_list->removeItem(i);
            i--;
        }
    }
    AlgoConfigData algo_data = ConfigManager::getInstance().getAlgoConfigData();
    for (const auto &algo : algo_data.items)
    {
        if (algo.type == AlgoType::noType)
            continue;
        if (ui->model_item_list->findText(algo.name) < 0 && !emit signalIsNameUsed(algo.name))
        {
            QVariant var;
            var.setValue(algo.uuid);
            ui->model_item_list->addItem(algo.name, var);
        }
    }
    if (original_text.isEmpty())
        ui->model_item_list->setCurrentIndex(-1);
}

bool ModelItem::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::Wheel)
        return true;

    return QWidget::eventFilter(watched, event);
}
}  // namespace vtk::display
