/****************************************************************************
 *  @file     modelsetting.cpp
 *  @brief    设置窗口单个模型设置窗口类
 *  @author   ZHU Leyan
 *  @email
 *  @version
 *  @date     2021.01.07
 *  Copyright (c) 2020 SmartMore All rights reserved.
 ****************************************************************************/
#include "modelsetting.h"
#include "ui_modelsetting.h"

#include <QFileDialog>

namespace vtk::display
{

#define ELIDE_WIDTH_EN 360  // 英文语言下不显示省略号的最大长度;
#define ELIDE_WIDTH_ZH 440  // 中文语言下不显示省略号的最大长度;

using namespace vtk::common;
ModelSetting::ModelSetting(QWidget *parent, int index, const QUuid &model_id, const QString &model_name,
                           const QString &model_path, AlgoType m_type)
    : QWidget(parent), ui(new Ui::ModelSetting), m_id(index), m_type(m_type), m_uuid(model_id)
{
    initWidgets(model_name, model_path);
    initConnections();
}

ModelSetting::~ModelSetting()
{
    delete ui;
}

void ModelSetting::initWidgets(const QString &model_name, const QString &model_path)
{
    ui->setupUi(this);
    if (!model_name.isEmpty())
    {
        ui->model_name_input->setText(model_name);
    }
    else
    {
        QString prefix;
        switch (m_type)
        {
            case AlgoType::detectionType:
                prefix = "Detection";
                break;
            case AlgoType::classificationType:
                prefix = "Classification";
                break;
            case AlgoType::ocrType:
                prefix = "OCR";
                break;
            case AlgoType::segmentationType:
                prefix = "Segmentation";
                break;
            default:
                break;
        }
        QString new_model_name = prefix + QString::number(m_id + 1);
        ui->model_name_input->setText(new_model_name);
    }
    QFontMetrics fontWidth(ui->model_path->font());
    QString elide_note;
    if (qApp->property("lang") == "en")
    {
        elide_note = fontWidth.elidedText(model_path, Qt::ElideRight, ELIDE_WIDTH_EN);
    }
    else
        elide_note = fontWidth.elidedText(model_path, Qt::ElideRight, ELIDE_WIDTH_ZH);
    ui->model_path->setText(elide_note);
    ui->model_path->setToolTip(model_path);
    ui->model_name_warning->hide();
    ui->model_path_warning->hide();
    ui->model_name_input->installEventFilter(this);
}

void ModelSetting::initConnections()
{
    connect(ui->model_name_input, &QLineEdit::editingFinished, this, [=] { disableFocus(); });
    connect(this, SIGNAL(signalDeleteModel(int)), parentWidget(), SLOT(slotDeleteModel(int)));
    connect(this, SIGNAL(signalCheckDuplicate(int)), parentWidget(), SLOT(slotCheckDuplicate(int)));
}

void ModelSetting::on_delete_model_btn_clicked()
{
    emit signalDeleteModel(m_id);
}

void ModelSetting::on_model_path_btn_clicked()
{
    QFileDialog fileDialog;
    QString filepath = fileDialog.getOpenFileName(this, tr("Import Model"), ".", "Model Files (*.smartmore)");
    if (!filepath.isEmpty())
    {
        QFontMetrics fontWidth(ui->model_path->font());
        QString elide_note;
        if (qApp->property("lang") == "en")
        {
            elide_note = fontWidth.elidedText(filepath, Qt::ElideRight, ELIDE_WIDTH_EN - 45);
        }
        else
            elide_note = fontWidth.elidedText(filepath, Qt::ElideRight, ELIDE_WIDTH_ZH - 53);
        ui->model_path->setText(elide_note);
        ui->model_path->setToolTip(filepath);
        ui->model_path_warning->hide();
    }
}

void ModelSetting::disableFocus()
{
    validateModelName();
    ui->model_name_input->clearFocus();
}

bool ModelSetting::validateModelSetting()
{
    int flag = 0;
    if (!validateModelName())
    {
        flag++;
    }
    if (!validateModelPath())
    {
        flag++;
    }
    if (flag)
        return false;
    return true;
}

bool ModelSetting::validateModelPath()
{
    auto path = ui->model_path->toolTip();
    if (path.isEmpty())
    {
        ui->model_path_warning->setText(tr("Please import the model."));
        ui->model_path_warning->show();
        return false;
    }
    QFile filename(path);
    if (!filename.exists())
    {
        ui->model_path_warning->setText(tr("Invalid model path."));
        ui->model_path_warning->show();
        return false;
    }
    ui->model_path_warning->hide();
    return true;
}

bool ModelSetting::validateModelName()
{
    if (ui->model_name_input->isEmpty())
    {
        ui->model_name_warning->setText(tr("Please enter the model name."));
        ui->model_name_warning->show();
        return false;
    }
    if (ui->model_name_input->containsSpecial())
    {
        ui->model_name_warning->setText(tr("No special character please."));
        ui->model_name_warning->show();
        return false;
    }
    if (ui->model_name_input->containsSpace())
    {
        ui->model_name_warning->setText(tr("No space character please."));
        ui->model_name_warning->show();
        return false;
    }
    if (ui->model_name_input->exceedLength(20))
    {
        ui->model_name_warning->setText(tr("The name length is between 1 and 20 characters."));
        ui->model_name_warning->show();
        return false;
    }

    bool is_duplicate = emit signalCheckDuplicate(m_id);
    if (is_duplicate)
    {
        ui->model_name_warning->setText(tr("Model name can't be duplicate."));
        ui->model_name_warning->show();
        return false;
    }
    ui->model_name_warning->hide();
    return true;
}

int ModelSetting::getId()
{
    return m_id;
}

void ModelSetting::setId(int index)
{
    m_id = index;
}

QString ModelSetting::getModelName()
{
    return ui->model_name_input->text();
}

void ModelSetting::setModelName(QString model_name)
{
    ui->model_name_input->setText(model_name);
}

QString ModelSetting::getModelPath()
{
    return ui->model_path->toolTip();
}

QUuid ModelSetting::getModelId()
{
    return m_uuid;
}

bool ModelSetting::eventFilter(QObject *wcg, QEvent *event)
{
    if (event->type() == QEvent::FocusIn && wcg == ui->model_name_input)
    {
        ui->model_name_warning->hide();
    }
    return QWidget::eventFilter(wcg, event);
}

}  // namespace vtk::display
