/****************************************************************************
 *  @file     modelstack.cpp
 *  @brief    设置窗口单类模型设置窗口类
 *  @author   ZHU Leyan
 *  @email
 *  @version
 *  @date     2021.01.07
 *  Copyright (c) 2020 SmartMore All rights reserved.
 ****************************************************************************/
#include "modelstack.h"
#include "ui_modelstack.h"
#include "uiframe/commonuiset/messagedialog/messagedialog.h"

#include <QDebug>

namespace vtk::display
{

using namespace vtk::common;
using ViewConfigData = vtk::config::ViewConfigData;
using ConfigManager = vtk::config::ConfigManager;

ModelStack::ModelStack(QWidget *parent) : QWidget(parent), ui(new Ui::ModelStack)
{
    initWidgets();
    initConnections();
}

ModelStack::~ModelStack()
{
    delete ui;
}

void ModelStack::setModelType(vtk::common::AlgoType type)
{
    m_type = type;
}

void ModelStack::initWidgets()
{
    ui->setupUi(this);
    m_model_stack_container = new QVBoxLayout();
    m_model_stack_container->setAlignment(Qt::AlignTop);
    m_model_stack_container->setContentsMargins(20, 20, 20, 20);
    m_model_stack_container->setSpacing(0);
    ui->model_stack_scrollwidget->setLayout(m_model_stack_container);
    m_add_model_widget = new AddModelWidget(this);
    m_model_stack_container->addWidget(m_add_model_widget);
}

void ModelStack::initConnections()
{
    connect(m_add_model_widget, &AddModelWidget::signalAddModel, this,
            [=] { addModel(QUuid::createUuid(), QString(""), QString("")); });
}

void ModelStack::addModel(const QUuid &model_id, const QString &model_name, const QString &model_path)
{
    int count = m_model_stack_container->count();
    QSharedPointer<ModelSetting> amodel(new ModelSetting(this, count - 1, model_id, model_name, model_path, m_type));
    m_model_stack_container->insertWidget(count - 1, amodel.data());
    m_model_settings.push_back(amodel);
}

bool ModelStack::checkModelNameUsed(QString name)
{
    ViewConfigData view_data = ConfigManager::getInstance().getViewConfigData();
    for (const auto &data : view_data)
    {
        if (data.algo.contains(name))
            return true;
    }
    return false;
}

void ModelStack::slotDeleteModel(int index)
{
    QString model_name = m_model_settings[index]->getModelName();
    if (checkModelNameUsed(model_name))
    {
        MessageDialog model_name_used_dialog(DialogType::WarningDialog,
                                             tr("The model is being used. Delete this model will also delete the model "
                                                "selected in the corresponding view. Still want to delete it?"),
                                             {Yes, No});
        int result = model_name_used_dialog.exec();
        if (result == QDialog::Rejected)
            return;
    }
    m_model_stack_container->removeWidget(m_model_settings[index].data());

    QVector<QSharedPointer<ModelSetting>> tmp;
    for (int i = 0; i < m_model_settings.size(); i++)
    {
        if (i != index)
        {
            tmp.push_back(m_model_settings[i]);
        }
    }
    m_model_settings = tmp;
    ViewConfigData view_data = ConfigManager::getInstance().getViewConfigData();
    for (auto &data : view_data)
    {
        if (data.algo.contains(model_name))
        {
            data.algo.removeAll(model_name);
        }
    }
    // 后面的元素向前移位
    for (int i = index; i < m_model_settings.size(); i++)
    {
        int original_id = m_model_settings[i]->getId();
        int new_id = original_id - 1;
        m_model_settings[i]->setId(new_id);
        QString test_name_prefix;
        switch (m_type)
        {
            case (AlgoType::detectionType):
                test_name_prefix = "Detection";
                break;
            case (AlgoType::ocrType):
                test_name_prefix = "OCR";
                break;
            case (AlgoType::classificationType):
                test_name_prefix = "Classification";
                break;
            case (AlgoType::segmentationType):
                test_name_prefix = "Segmentation";
                break;
        }

        QString test_name = test_name_prefix + QString::number(original_id + 1);
        QString current_view_name = m_model_settings[i]->getModelName();
        QString new_view_name = current_view_name;
        if (current_view_name == test_name)
            new_view_name = test_name_prefix + QString::number(new_id + 1);
        m_model_settings[i]->setModelName(new_view_name);
        for (auto &data : view_data)
        {
            for (int j = 0; j < data.algo.size(); j++)
            {
                if (data.algo[j] == test_name)
                {
                    data.algo.replace(j, new_view_name);
                }
            }
        }
    }
    ConfigManager::getInstance().setViewConfigData(view_data);
}

bool ModelStack::slotCheckDuplicate(int index)
{
    for (int i = 0; i < m_model_settings.size(); i++)
    {
        if (i == index)
            continue;
        if (m_model_settings[i]->getModelName() == m_model_settings[index]->getModelName())
        {
            return true;
        }
    }
    return emit signalCheckDuplicate(m_model_settings[index]->getModelName());
}

QStringList ModelStack::getModelNames()
{
    QStringList model_names;
    for (int i = 0; i < m_model_settings.size(); i++)
    {
        model_names.push_back(m_model_settings[i]->getModelName());
    }
    return model_names;
}

QVector<AlgoConfigItem> ModelStack::getModelData()
{
    QVector<AlgoConfigItem> model_data;
    for (int i = 0; i < m_model_settings.size(); i++)
    {
        AlgoConfigItem data_item;
        data_item.uuid = m_model_settings[i]->getModelId();
        data_item.name = m_model_settings[i]->getModelName();
        data_item.type = m_type;
        data_item.model_file = m_model_settings[i]->getModelPath();
        model_data.push_back(data_item);
    }
    return model_data;
}

void ModelStack::clearModels()
{
    m_model_settings.clear();
}

void ModelStack::disableFocus()
{
    for (int i = 0; i < m_model_settings.size(); i++)
    {
        m_model_settings[i]->disableFocus();
    }
}

bool ModelStack::validatePage()
{
    int flag = 0;
    for (int i = 0; i < m_model_settings.size(); i++)
    {
        if (!m_model_settings[i]->validateModelSetting())
            flag++;
    }
    if (flag)
        return false;
    return true;
}
}  // namespace vtk::display
