/****************************************************************************
 *  @file     modelsettingpage.cpp
 *  @brief    设置窗口模型设置界面类
 *  @author
 *  @email
 *  @version
 *  @date     2020.12.21
 *  Copyright (c) 2020 SmartMore All rights reserved.
 ****************************************************************************/
#include "modelsettingpage.h"
#include "common/vtkcommon.h"
#include "configdatamanager/configmanager.h"
#include "ui_modelsettingpage.h"
#include "uiframe/commonuiset/messagedialog/messagedialog.h"

#include <QDebug>
#include <QFileDialog>
#include <QSettings>
#include <QStyle>
#include <QUuid>

namespace vtk::display
{
using ConfigManager = vtk::config::ConfigManager;
using AlgoConfigData = vtk::config::AlgoConfigData;
using AlgoConfigItem = vtk::config::AlgoConfigItem;
using AlgoType = vtk::common::AlgoType;

ModelSettingPage::ModelSettingPage(QWidget *parent) : QWidget(parent), ui(new Ui::ModelSettingPage)
{
    initWidgets();
    initConnections();
}

ModelSettingPage::~ModelSettingPage()
{
    delete ui;
}

void ModelSettingPage::initWidgets()
{
    ui->setupUi(this);
    ui->detection_algo_tab->setProperty("status", "selected");
    ui->classification_algo_tab->setProperty("status", "unselected");
    ui->segmentation_algo_tab->setProperty("status", "unselected");
    ui->ocr_algo_tab->setProperty("status", "unselected");
    style()->polish(ui->detection_algo_tab);
    style()->polish(ui->classification_algo_tab);
    style()->polish(ui->segmentation_algo_tab);
    style()->polish(ui->ocr_algo_tab);

    QLabel *textLabel = new QLabel;
    textLabel->setStyleSheet("color: white; font-size: 14px;background: rgb(100,100,100);");
    textLabel->setWordWrap(true);
    textLabel->setText(tr("The CPU mode will be used for image processing when the button is closed. The Nvidia GPU "
                          "mode will be used for image processing when the button is open."));

    m_instruction = new ArrowWidget(this);
    m_instruction->setTriangleInfo(14, 8);
    m_instruction->setCenterWidget(textLabel);
    m_instruction->hide();

    ui->detection_stack->setModelType(AlgoType::detectionType);
    ui->ocr_stack->setModelType(AlgoType::ocrType);
    ui->classification_stack->setModelType(AlgoType::classificationType);
    ui->segmentation_stack->setModelType(AlgoType::segmentationType);
    this->installEventFilter(this);
}

void ModelSettingPage::initConnections()
{
    connect(ui->gpu_btn, SIGNAL(clicked(bool)), this, SLOT(slotToggleGpu(bool)));
    connect(ui->question_mark_btn, SIGNAL(clicked()), this, SLOT(slotShowInstruction()));
    connect(ui->detection_stack, &ModelStack::signalCheckDuplicate, this,
            [&](QString name) { return checkDuplicate(AlgoType::detectionType, name); });
    connect(ui->ocr_stack, &ModelStack::signalCheckDuplicate, this,
            [&](QString name) { return checkDuplicate(AlgoType::ocrType, name); });
    connect(ui->classification_stack, &ModelStack::signalCheckDuplicate, this,
            [&](QString name) { return checkDuplicate(AlgoType::classificationType, name); });
    connect(ui->segmentation_stack, &ModelStack::signalCheckDuplicate, this,
            [&](QString name) { return checkDuplicate(AlgoType::segmentationType, name); });
}

void ModelSettingPage::cancelModelConfig()
{
    ui->detection_stack->clearModels();
    ui->ocr_stack->clearModels();
    ui->classification_stack->clearModels();
    ui->segmentation_stack->clearModels();
    initModelConfigData();
}

void ModelSettingPage::initModelConfigData()
{
    AlgoConfigData algo_data = ConfigManager::getInstance().getAlgoConfigData();
    for (int i = 0; i < algo_data.items.size(); i++)
    {
        if (algo_data.items[i].type == AlgoType::detectionType)
        {
            ui->detection_stack->addModel(algo_data.items[i].uuid, algo_data.items[i].name,
                                          algo_data.items[i].model_file);
        }
        else if (algo_data.items[i].type == AlgoType::ocrType)
        {
            ui->ocr_stack->addModel(algo_data.items[i].uuid, algo_data.items[i].name, algo_data.items[i].model_file);
        }
        else if (algo_data.items[i].type == AlgoType::classificationType)
        {
            ui->classification_stack->addModel(algo_data.items[i].uuid, algo_data.items[i].name,
                                               algo_data.items[i].model_file);
        }
        else if (algo_data.items[i].type == AlgoType::segmentationType)
        {
            ui->segmentation_stack->addModel(algo_data.items[i].uuid, algo_data.items[i].name,
                                             algo_data.items[i].model_file);
        }
    }

    bool is_gpu_on = algo_data.gpu_mode;
    if (is_gpu_on)
    {
        ui->gpu_btn->setChecked(true);
    }
    else
    {
        ui->gpu_btn->setChecked(false);
    }

    emit signalUpdateModelData();
}

bool ModelSettingPage::validateModelSettings()
{
    int flag = 0;
    if (!ui->detection_stack->validatePage())
        flag++;
    if (!ui->ocr_stack->validatePage())
        flag++;
    if (!ui->classification_stack->validatePage())
        flag++;
    if (!ui->segmentation_stack->validatePage())
        flag++;
    if (flag)
        return false;
    return true;
}

void ModelSettingPage::slotShowInstruction()
{
    int x = ui->question_mark_btn->geometry().x();
    if (qApp->property("lang") == "zh")  // chinese
    {
        m_instruction->setGeometry(x - 82, 100, 0, 0);
        m_instruction->setStartPos(123);
        m_instruction->setFixedSize(QSize(260, 80));
    }
    else  // english
    {
        m_instruction->setGeometry(x - 92, 100, 0, 0);
        m_instruction->setStartPos(133);
        m_instruction->setFixedSize(QSize(280, 140));
    }
    m_instruction->show();
}

void ModelSettingPage::slotToggleGpu(bool checked)
{
    (void)checked;
    if (ui->gpu_btn->isChecked())
    {
        MessageDialog gpu_dialog(DialogType::WarningDialog,
                                 tr("Please check whether your device has an NVIDIA GPU with 8G memory. "
                                    "If it is not installed, the software may crash unexpectedly."));
        gpu_dialog.exec();
    }
}

bool ModelSettingPage::checkDuplicate(AlgoType type, QString name)
{
    QStringList detection_names = getModelNames(AlgoType::detectionType);
    QStringList classification_names = getModelNames(AlgoType::classificationType);
    QStringList ocr_names = getModelNames(AlgoType::ocrType);
    QStringList segmentation_names = getModelNames(AlgoType::segmentationType);
    switch (type)
    {
        case AlgoType::classificationType:
            if (detection_names.contains(name))
                return true;
            if (ocr_names.contains(name))
                return true;
            if (segmentation_names.contains(name))
                return true;
            return false;
        case AlgoType::ocrType:
            if (detection_names.contains(name))
                return true;
            if (classification_names.contains(name))
                return true;
            if (segmentation_names.contains(name))
                return true;
            return false;
        case AlgoType::segmentationType:
            if (detection_names.contains(name))
                return true;
            if (ocr_names.contains(name))
                return true;
            if (classification_names.contains(name))
                return true;
            return false;
        case AlgoType::detectionType:
            if (classification_names.contains(name))
                return true;
            if (ocr_names.contains(name))
                return true;
            if (segmentation_names.contains(name))
                return true;
            return false;
    }
}

void ModelSettingPage::saveModelConfigData()
{
    AlgoConfigData data;
    data.items.append(ui->detection_stack->getModelData());
    data.items.append(ui->classification_stack->getModelData());
    data.items.append(ui->ocr_stack->getModelData());
    data.items.append(ui->segmentation_stack->getModelData());
    if (ui->gpu_btn->isChecked())
        data.gpu_mode = true;
    else
        data.gpu_mode = false;
    ConfigManager::getInstance().setAlgoConfigData(data);
}

void ModelSettingPage::on_detection_algo_tab_clicked()
{
    switchTab(0);
}

void ModelSettingPage::on_classification_algo_tab_clicked()
{
    switchTab(1);
}

void ModelSettingPage::on_segmentation_algo_tab_clicked()
{
    switchTab(2);
}

void ModelSettingPage::on_ocr_algo_tab_clicked()
{
    switchTab(3);
}

bool ModelSettingPage::validatePage(int index)
{
    if (index == 0)
        return ui->detection_stack->validatePage();
    else if (index == 1)
        return ui->classification_stack->validatePage();
    else if (index == 2)
        return ui->segmentation_stack->validatePage();
    else
        return ui->ocr_stack->validatePage();
}

void ModelSettingPage::switchTab(int index)
{
    // first validate
    if (!validatePage(ui->model_stacks->currentIndex()))
        return;
    ui->model_stacks->setCurrentIndex(index);
    if (index == 0)
    {
        ui->detection_algo_tab->setProperty("status", "selected");
        ui->classification_algo_tab->setProperty("status", "unselected");
        ui->segmentation_algo_tab->setProperty("status", "unselected");
        ui->ocr_algo_tab->setProperty("status", "unselected");
    }
    else if (index == 1)
    {
        ui->detection_algo_tab->setProperty("status", "unselected");
        ui->classification_algo_tab->setProperty("status", "selected");
        ui->segmentation_algo_tab->setProperty("status", "unselected");
        ui->ocr_algo_tab->setProperty("status", "unselected");
    }
    else if (index == 2)
    {
        ui->detection_algo_tab->setProperty("status", "unselected");
        ui->classification_algo_tab->setProperty("status", "unselected");
        ui->segmentation_algo_tab->setProperty("status", "selected");
        ui->ocr_algo_tab->setProperty("status", "unselected");
    }
    else if (index == 3)
    {
        ui->detection_algo_tab->setProperty("status", "unselected");
        ui->classification_algo_tab->setProperty("status", "unselected");
        ui->segmentation_algo_tab->setProperty("status", "unselected");
        ui->ocr_algo_tab->setProperty("status", "selected");
    }
    style()->polish(ui->detection_algo_tab);
    style()->polish(ui->classification_algo_tab);
    style()->polish(ui->segmentation_algo_tab);
    style()->polish(ui->ocr_algo_tab);
}

QStringList ModelSettingPage::getModelNames(AlgoType algo_type)
{
    switch (algo_type)
    {
        case AlgoType::classificationType:
            return ui->classification_stack->getModelNames();
        case AlgoType::ocrType:
            return ui->ocr_stack->getModelNames();
        case AlgoType::detectionType:
            return ui->detection_stack->getModelNames();
        case AlgoType::segmentationType:
            return ui->segmentation_stack->getModelNames();
    }
}

void ModelSettingPage::disableFocus()
{
    ui->detection_stack->disableFocus();
    ui->classification_stack->disableFocus();
    ui->ocr_stack->disableFocus();
    ui->segmentation_stack->disableFocus();
}

bool ModelSettingPage::eventFilter(QObject *wcg, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress)
    {
        m_instruction->hide();
    }
    return QWidget::eventFilter(wcg, event);
}
}  // namespace vtk::display
