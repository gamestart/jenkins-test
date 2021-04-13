/****************************************************************************
 *  @file     savesetting.cpp
 *  @brief    设置窗口单个图片保存设置界面类
 *  @author   ZHU Leyan
 *  @email
 *  @version
 *  @date     2021.01.18
 *  Copyright (c) 2020 SmartMore All rights reserved.
 ****************************************************************************/
#include "savesetting.h"
#include "ui_savesetting.h"

#include <QFileDialog>
#include <QListView>

namespace vtk::display
{
#define ELIDE_WIDTH_EN 190  // 英文语言下不显示省略号的最大长度;
#define ELIDE_WIDTH_ZH 230  // 中文语言下不显示省略号的最大长度;

SaveSetting::SaveSetting(QWidget *parent) : QWidget(parent), ui(new Ui::SaveSetting)
{
    ui->setupUi(this);
    ui->save_format_list->setView(new QListView());
    ui->save_number_input->installEventFilter(this);
    ui->save_day_input->installEventFilter(this);
    initConnections();
}

SaveSetting::~SaveSetting()
{
    delete ui;
}

void SaveSetting::setTitle(QString title)
{
    ui->save_pic_title->setText(title);
}

void SaveSetting::initConnections()
{
    connect(ui->save_pic_btn, SIGNAL(clicked(bool)), this, SLOT(slotToggleSavePic(bool)));
    connect(ui->save_ok_btn, SIGNAL(clicked(bool)), this, SLOT(slotToggleSaveOK(bool)));
    connect(ui->save_ng_btn, SIGNAL(clicked(bool)), this, SLOT(slotToggleSaveNG(bool)));
    connect(ui->delete_condition_btn, SIGNAL(clicked(bool)), this, SLOT(slotToggleDeleteCondition(bool)));
    connect(ui->ok_path_btn, &QToolButton::clicked, this, [=] { selectPath(1); });
    connect(ui->ng_path_btn, &QToolButton::clicked, this, [=] { selectPath(2); });
    connect(ui->save_number_input, &QLineEdit::editingFinished, this, [=] { disableFocus(1); });
    connect(ui->save_day_input, &QLineEdit::editingFinished, this, [=] { disableFocus(2); });
}

void SaveSetting::initImageSaveConfigData(const SaveConfigItem &data)
{
    m_image_save_config_data = data;
    ui->save_format_list->setCurrentText(data.pic_format);
    QFontMetrics ok_font_width(ui->ok_path->font());
    QString ok_elide_note;
    if (qApp->property("lang") == "en")
        ok_elide_note = ok_font_width.elidedText(data.ok_path, Qt::ElideRight, ELIDE_WIDTH_EN);
    else
        ok_elide_note = ok_font_width.elidedText(data.ok_path, Qt::ElideRight, ELIDE_WIDTH_ZH);
    ui->ok_path->setText(ok_elide_note);
    ui->ok_path->setToolTip(data.ok_path);
    QFontMetrics ng_font_width(ui->ng_path->font());
    QString ng_elide_note;
    if (qApp->property("lang") == "en")
        ng_elide_note = ng_font_width.elidedText(data.ok_path, Qt::ElideRight, ELIDE_WIDTH_EN);
    else
        ng_elide_note = ng_font_width.elidedText(data.ok_path, Qt::ElideRight, ELIDE_WIDTH_ZH);
    ui->ng_path->setText(ng_elide_note);
    ui->ng_path->setToolTip(data.ng_path);
    ui->save_number_input->setText(QString::number(data.save_number));
    ui->save_day_input->setText(QString::number(data.save_day));

    ui->save_pic_btn->setChecked(data.save_pic);
    if (data.save_pic)
    {
        ui->save_ok_btn->setChecked(data.save_ok);
        ui->save_ng_btn->setChecked(data.save_ng);
        ui->delete_condition_btn->setChecked(data.delete_pic);
        enableSavePic(true);
        if (data.save_ok)
            enableSaveOK(true);
        else
            disableSaveOK();
        if (data.save_ng)
            enableSaveNG(true);
        else
            disableSaveNG();
        if (data.delete_pic)
            enableDelete();
        else
            disableDelete();
    }
    else
    {
        ui->save_ok_btn->setChecked(false);
        ui->save_ng_btn->setChecked(false);
        ui->delete_condition_btn->setChecked(false);
        disableSavePic();
    }
    ui->save_day_warning->setText("");
    ui->save_number_warning->setText("");
}

void SaveSetting::selectPath(int index)
{
    QFileDialog file_dialog;
    if (index == 1)
    {
        QString cpath = ui->ok_path->toolTip();
        QDir dir(cpath);
        if (!dir.exists())
        {
            dir.mkpath(cpath);
        }
        QString filepath = file_dialog.getExistingDirectory(this, "Select Directory", cpath, QFileDialog::ShowDirsOnly);
        if (filepath != QString(""))
        {
            QFontMetrics font_width(ui->ok_path->font());
            QString elide_note;
            if (qApp->property("lang") == "en")
                elide_note = font_width.elidedText(filepath, Qt::ElideRight, ELIDE_WIDTH_EN);
            else
                elide_note = font_width.elidedText(filepath, Qt::ElideRight, ELIDE_WIDTH_ZH);
            ui->ok_path->setText(elide_note);
            ui->ok_path->setToolTip(filepath);
        }
    }
    else
    {
        QString cpath = ui->ng_path->toolTip();
        QDir dir(cpath);
        if (!dir.exists())
        {
            dir.mkpath(cpath);
        }
        QString filepath = file_dialog.getExistingDirectory(this, "Select Directory", cpath, QFileDialog::ShowDirsOnly);
        if (filepath != QString(""))
        {
            QFontMetrics font_width(ui->ng_path->font());
            QString elide_note;
            if (qApp->property("lang") == "en")
                elide_note = font_width.elidedText(filepath, Qt::ElideRight, ELIDE_WIDTH_EN);
            else
                elide_note = font_width.elidedText(filepath, Qt::ElideRight, ELIDE_WIDTH_ZH);
            ui->ok_path->setText(elide_note);
            ui->ng_path->setToolTip(filepath);
        }
    }
}

void SaveSetting::enableSavePic(bool init)
{
    enableSaveOK(init);
    enableSaveNG(init);
    enableDelete();
    ui->save_type_and_path_title->setProperty("status", "enabled");
    ui->save_ok_btn->setEnabled(true);
    ui->save_ng_btn->setEnabled(true);
    ui->save_ok_title->setProperty("status", "enabled");
    ui->save_ng_title->setProperty("status", "enabled");
    ui->delete_condition_title->setProperty("status", "enabled");
    ui->delete_condition_btn->setEnabled(true);
    ui->save_format_title->setProperty("status", "enabled");
    ui->save_format_list->setEnabled(true);
    ui->save_format_list->setProperty("status", "enabled");
    style()->polish(ui->save_type_and_path_title);
    style()->polish(ui->save_ok_title);
    style()->polish(ui->save_ng_title);
    style()->polish(ui->delete_condition_title);
    style()->polish(ui->save_format_title);
    style()->polish(ui->save_format_list);
}

void SaveSetting::disableSavePic()
{
    disableSaveOK();
    disableSaveNG();
    disableDelete();
    ui->save_type_and_path_title->setProperty("status", "disabled");
    ui->save_ok_btn->setEnabled(false);
    ui->save_ng_btn->setEnabled(false);
    ui->save_ok_title->setProperty("status", "disabled");
    ui->save_ng_title->setProperty("status", "disabled");
    ui->delete_condition_title->setProperty("status", "disabled");
    ui->delete_condition_btn->setEnabled(false);
    ui->save_format_title->setProperty("status", "disabled");
    ui->save_format_list->setEnabled(false);
    ui->save_format_list->setProperty("status", "disabled");
    style()->polish(ui->save_type_and_path_title);
    style()->polish(ui->save_ok_title);
    style()->polish(ui->save_ng_title);
    style()->polish(ui->delete_condition_title);
    style()->polish(ui->save_format_title);
    style()->polish(ui->save_format_list);
}

void SaveSetting::enableSaveNG(bool init)
{
    ui->ng_path_btn->setEnabled(true);
    ui->ng_path->setProperty("status", "enabled");
    ui->ng_path_btn->setProperty("status", "enabled");
    style()->polish(ui->ng_path);
    style()->polish(ui->ng_path_btn);

    QString path = m_image_save_config_data.ng_path;
    QDir dir(path);
    if (path.isEmpty() || !dir.exists())
    {
        // ui->ng_path->setText(qApp->applicationDirPath() + "/data/NG");
        // ui->ng_path->setToolTip(qApp->applicationDirPath() + "/data/NG");
    }
}

void SaveSetting::enableSaveOK(bool init)
{
    ui->ok_path_btn->setEnabled(true);
    ui->ok_path->setProperty("status", "enabled");
    ui->ok_path_btn->setProperty("status", "enabled");
    style()->polish(ui->ok_path);
    style()->polish(ui->ok_path_btn);

    QString path = m_image_save_config_data.ok_path;
    QDir dir(path);
    if (path.isEmpty() || !dir.exists())
    {
        // ui->ok_path->setText(qApp->applicationDirPath() + "/data/OK");
        // ui->ok_path->setToolTip(qApp->applicationDirPath() + "/data/OK");
    }
}

void SaveSetting::disableSaveOK()
{
    ui->ok_path_btn->setEnabled(false);
    ui->ok_path->setProperty("status", "disabled");
    ui->ok_path_btn->setProperty("status", "disabled");
    style()->polish(ui->ok_path);
    style()->polish(ui->ok_path_btn);
}

void SaveSetting::disableSaveNG()
{
    ui->ng_path_btn->setEnabled(false);
    ui->ng_path->setProperty("status", "disabled");
    ui->ng_path_btn->setProperty("status", "disabled");
    style()->polish(ui->ng_path);
    style()->polish(ui->ng_path_btn);
}

void SaveSetting::enableDelete()
{
    ui->save_number_input->setEnabled(true);
    ui->save_day_input->setEnabled(true);

    ui->save_number_title->setProperty("status", "enabled");
    ui->save_number_input->setProperty("status", "enabled");
    ui->save_day_title->setProperty("status", "enabled");
    ui->save_day_input->setProperty("status", "enabled");
    style()->polish(ui->save_number_title);
    style()->polish(ui->save_number_input);
    style()->polish(ui->save_day_title);
    style()->polish(ui->save_day_input);
}

void SaveSetting::disableDelete()
{
    ui->save_number_input->setEnabled(false);
    ui->save_day_input->setEnabled(false);

    ui->save_number_title->setProperty("status", "disabled");
    ui->save_number_input->setProperty("status", "disabled");
    ui->save_day_title->setProperty("status", "disabled");
    ui->save_day_input->setProperty("status", "disabled");
    style()->polish(ui->save_number_title);
    style()->polish(ui->save_number_input);
    style()->polish(ui->save_day_title);
    style()->polish(ui->save_day_input);

    ui->save_number_warning->setText("");
    ui->save_day_warning->setText("");
}

void SaveSetting::slotToggleSavePic(bool checked)
{
    if (checked)
    {
        ui->save_ok_btn->setChecked(true);
        ui->save_ng_btn->setChecked(true);
        ui->delete_condition_btn->setChecked(true);
        enableSavePic(false);
    }
    else
    {
        ui->save_ok_btn->setChecked(false);
        ui->save_ng_btn->setChecked(false);
        ui->delete_condition_btn->setChecked(false);
        disableSavePic();
    }
}

void SaveSetting::slotToggleSaveOK(bool checked)
{
    if (checked)
    {
        enableSaveOK(false);
    }
    else
    {
        disableSaveOK();
    }
}

void SaveSetting::slotToggleSaveNG(bool checked)
{
    if (checked)
    {
        enableSaveNG(false);
    }
    else
    {
        disableSaveNG();
    }
}

void SaveSetting::slotToggleDeleteCondition(bool checked)
{
    if (checked)
    {
        enableDelete();
    }
    else
    {
        disableDelete();
    }
}

const SaveConfigItem &SaveSetting::saveImageSaveConfigData()
{
    m_image_save_config_data.save_pic = ui->save_pic_btn->isChecked();
    m_image_save_config_data.save_ok = ui->save_ok_btn->isChecked();
    m_image_save_config_data.save_ng = ui->save_ng_btn->isChecked();
    m_image_save_config_data.delete_pic = ui->delete_condition_btn->isChecked();
    m_image_save_config_data.ok_path = ui->ok_path->toolTip();
    m_image_save_config_data.ng_path = ui->ng_path->toolTip();
    m_image_save_config_data.pic_format = ui->save_format_list->currentText();
    if (ui->save_number_input->isEmpty())
        m_image_save_config_data.save_number = 1000;
    else
        m_image_save_config_data.save_number = ui->save_number_input->text().toInt();
    if (ui->save_day_input->isEmpty())
        m_image_save_config_data.save_day = 30;
    else
        m_image_save_config_data.save_day = ui->save_day_input->text().toInt();
    return m_image_save_config_data;
}

void SaveSetting::disableFocus(int index)
{
    if (index == 1)
    {
        validateSaveNumber();
        ui->save_number_input->clearFocus();
    }
    else
    {
        validateSaveDay();
        ui->save_day_input->clearFocus();
    }
}

bool SaveSetting::validateSaveSetting()
{
    int flag = 0;
    if (!validateSaveNumber())
        flag++;
    if (!validateSaveDay())
        flag++;
    if (flag)
        return false;
    else
        return true;
}

bool SaveSetting::validateSaveNumber()
{
    if (ui->save_number_input->isNum())
    {
        if (ui->save_number_input->isInt())
        {
            if (ui->save_number_input->toInt() <= 0)
            {
                ui->save_number_warning->setText(tr("Please enter positive number."));
                return false;
            }
            else
            {
                ui->save_number_warning->setText("");
                return true;
            }
        }
        else
        {
            ui->save_number_warning->setText(tr("Number is too large."));
            return false;
        }
    }
    else
    {
        if (ui->save_number_input->isEnabled())
        {
            ui->save_number_warning->setText(tr("Please enter the number."));
            return false;
        }
        else
        {
            ui->save_number_warning->setText("");
            return true;
        }
    }
}

bool SaveSetting::validateSaveDay()
{
    if (ui->save_day_input->isNum())
    {
        if (ui->save_day_input->isInt())
        {
            if (ui->save_day_input->toInt() <= 0)
            {
                ui->save_day_warning->setText(tr("Please enter positive number."));
                return false;
            }
            else
            {
                ui->save_day_warning->setText("");
                return true;
            }
        }
        else
        {
            ui->save_day_warning->setText(tr("Number is too large."));
            return false;
        }
    }
    else
    {
        if (ui->save_day_input->isEnabled())
        {
            ui->save_day_warning->setText(tr("Please enter the number."));
            return false;
        }
        else
        {
            ui->save_day_warning->setText("");
            return true;
        }
    }
}

bool SaveSetting::eventFilter(QObject *wcg, QEvent *event)
{
    if (event->type() == QEvent::FocusIn)
    {
        if (wcg == ui->save_number_input)
            ui->save_number_warning->setText("");
        else if (wcg == ui->save_day_input)
            ui->save_day_warning->setText("");
    }
    return QWidget::eventFilter(wcg, event);
}
}  // namespace vtk::display
