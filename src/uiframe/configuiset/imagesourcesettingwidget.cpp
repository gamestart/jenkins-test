/****************************************************************************
 *  @file     imagesourcesettingwidget.cpp
 *  @brief    图像数据源设置窗口部件类
 *  @author   junjie.zeng
 *  @email    junjie.zeng@smartmore.com
 *  @version
 *  @date     2021.01.08
 *  Copyright (c) 2021 SmartMore All rights reserved.
 ****************************************************************************/
#include "imagesourcesettingwidget.h"
#include "ui_imagesourcesettingwidget.h"
#include "uiframe/commonuiset/messagedialog/messagedialog.h"
#include <QListView>
#include <QRegExpValidator>

namespace vtk::display
{

using ConfigManager = vtk::config::ConfigManager;
using TriggerMode = vtk::config::TriggerMode;
using Message = vtk::common::Message;

ImageSourceSettingWidget::ImageSourceSettingWidget(const SourceConfigItem &data, QWidget *parent)
    : QWidget(parent), ui(new Ui::ImageSourceSettingWidget), m_data(data)
{
    init();
}

ImageSourceSettingWidget::~ImageSourceSettingWidget()
{
    delete ui;
}

void ImageSourceSettingWidget::resetTriggerLevelOrderCombox(const QStringList &string_list, const QString &current_text)
{
    ui->trigger_level_order_combox->blockSignals(true);
    ui->trigger_level_order_combox->clear();
    foreach (const auto &str, string_list)
        ui->trigger_level_order_combox->addItem(str);
    if (current_text.isEmpty() || !string_list.contains(current_text))
    {
        ui->trigger_level_order_combox->setCurrentIndex(0);
        if (!current_text.isEmpty())
            m_data.trigger_level_order = ui->trigger_level_order_combox->currentText();
    }
    else
        ui->trigger_level_order_combox->setCurrentText(current_text);
    ui->trigger_level_order_combox->blockSignals(false);
    emit ui->trigger_level_order_combox->currentTextChanged(ui->trigger_level_order_combox->currentText());
}

const QString &ImageSourceSettingWidget::getCurrentImageSourceName() const
{
    return m_data.source_name;
}

void ImageSourceSettingWidget::setCurrentImageSourceName(const QString &source_name)
{
    m_data.source_name = source_name;
    ui->source_name_lineedit->setText(m_data.source_name);
    ui->image_source_name->setText(m_data.source_name);
}

const QString &ImageSourceSettingWidget::getCurrentImageSource() const
{
    return m_data.source;
}

const QString &ImageSourceSettingWidget::getCurrentTriggerChar() const
{
    return m_data.trigger_char;
}

void ImageSourceSettingWidget::setCurrentTriggerChar(const QString &trigger_char)
{
    m_data.trigger_char = trigger_char;
    ui->trigger_character_lineedit->setText(m_data.trigger_char);
}

const QString &ImageSourceSettingWidget::getCurrentTriggerLevel() const
{
    return m_data.trigger_level;
}

const QString &ImageSourceSettingWidget::getCurrentTriggerLevelOrder() const
{
    return m_data.trigger_level_order;
}

const SourceConfigItem &ImageSourceSettingWidget::getSourceConfigData() const
{
    return m_data;
}

bool ImageSourceSettingWidget::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::Wheel)
        return true;
    if (event->type() == QEvent::FocusIn)
    {
        if (watched == ui->source_name_lineedit)
            ui->source_name_warning->setText("");
        if (watched == ui->trigger_character_lineedit)
            ui->trigger_character_warning->setText("");
    }

    return QWidget::eventFilter(watched, event);
}

void ImageSourceSettingWidget::init()
{
    initWidgets();
    initConnects();
}

void ImageSourceSettingWidget::initWidgets()
{
    ui->setupUi(this);
    if (qApp->property("lang") == "en")
    {
        ui->body_layout->setHorizontalSpacing(10);
    }
    auto source_list_view = new QListView();
    source_list_view->setIconSize(QSize(6, 6));
    ui->image_source_combox->setView(source_list_view);
    ui->trigger_level_combox->setView(new QListView());
    ui->trigger_level_order_combox->setView(new QListView());
    ui->source_name_lineedit->setText(m_data.source_name);
    ui->image_source_name->setText(m_data.source_name);

    const auto &data_list = ConfigManager::getInstance().getCameraConfigData();
    foreach (const auto &data, data_list)
    {
        auto status = vtk::common::CameraStatusGetter::getInstance().signalGetCameraStatus(data.name);
        QString icon_url;
        switch (status)
        {
            case vtk::common::CameraState::closed:
                icon_url = QString(":/uiframe/images/viewtoolbar/camera_off.svg");
                break;
            case vtk::common::CameraState::opening:
                icon_url = QString(":/uiframe/images/viewtoolbar/camera_on.svg");
                break;
            case vtk::common::CameraState::disconnect:
                icon_url = QString(":/uiframe/images/viewtoolbar/camera_disconnect.svg");
                break;
        }
        SourceInfo info = {data.id, status};
        ui->image_source_combox->addItem(QIcon(icon_url), data.name, QVariant::fromValue(info));
    }
    ui->image_source_combox->setCurrentText(m_data.source);
    ui->trigger_character_lineedit->setText(m_data.trigger_char);
    ui->trigger_level_combox->setCurrentIndex(ui->trigger_level_combox->findText(m_data.trigger_level));
    if (ConfigManager::getInstance().getTriggerMode() == TriggerMode::hardTrigger)
        ui->trigger_level_combox->setEnabled(false);

    ui->image_source_combox->installEventFilter(this);
    ui->trigger_level_combox->installEventFilter(this);
    ui->trigger_level_order_combox->installEventFilter(this);
    ui->source_name_lineedit->installEventFilter(this);
    ui->trigger_character_lineedit->installEventFilter(this);
}

void ImageSourceSettingWidget::disableFocus(int index)
{
    if (index == 1)
    {
        if (validateSourceName())
            ui->image_source_name->setText(ui->source_name_lineedit->text());
        ui->source_name_lineedit->clearFocus();
    }
    else if (index == 2)
    {
        validateTriggerCharacter();
        ui->trigger_character_lineedit->clearFocus();
    }
}

bool ImageSourceSettingWidget::validateSourceSetting()
{
    int flag = 0;
    if (!validateSourceName())
        flag++;
    if (!validateTriggerCharacter())
        flag++;
    if (flag)
        return false;
    else
        return true;
}

bool ImageSourceSettingWidget::validateSourceName()
{
    if (ui->source_name_lineedit->isEmpty())
    {
        ui->source_name_warning->setText(tr("Please enter the source name."));
        return false;
    }
    if (ui->source_name_lineedit->containsSpecial())
    {
        ui->source_name_warning->setText(tr("No special character please."));
        return false;
    }
    if (ui->source_name_lineedit->containsSpace())
    {
        ui->source_name_warning->setText(tr("No space character please."));
        return false;
    }
    if (ui->source_name_lineedit->exceedLength(10))
    {
        ui->source_name_warning->setText(tr("The name length is between 1 and 10 characters."));
        return false;
    }
    const auto new_name = ui->source_name_lineedit->text();
    if (new_name != m_data.source_name && !signalSourceNameTextChanged(m_data.source_name, new_name))
    {
        ui->source_name_warning->setText(tr("Source name can't be duplicate."));
        return false;
    }
    ui->source_name_warning->setText("");
    m_data.source_name = ui->source_name_lineedit->text();
    return true;
}

bool ImageSourceSettingWidget::validateTriggerCharacter()
{
    if (ui->trigger_character_lineedit->isEmpty())
    {
        ui->trigger_character_warning->setText(tr("Please enter the trigger character."));
        return false;
    }
    if (ui->trigger_character_lineedit->exceedLength(10))
    {
        ui->trigger_character_warning->setText(
            tr("The length of the trigger character should be between 1 and 10 characters."));
        return false;
    }
    if (ui->trigger_character_lineedit->containsChinese())
    {
        ui->trigger_character_warning->setText(tr("No Chinese character please."));
        return false;
    }
    const auto new_char = ui->trigger_character_lineedit->text();
    if (new_char != m_data.trigger_char && !signalTriggerCharTextChanged(m_data.trigger_char, new_char))
    {
        ui->trigger_character_warning->setText(tr("Trigger character can't be duplicate."));
        return false;
    }
    ui->trigger_character_warning->setText("");
    m_data.trigger_char = ui->trigger_character_lineedit->text();
    return true;
}

void ImageSourceSettingWidget::initConnects()
{
    connect(ui->source_name_lineedit, &QLineEdit::editingFinished, this, [&] { disableFocus(1); });
    connect(ui->image_source_combox, &QComboBox::currentTextChanged, this, [&](const QString &source) {
        if (source == m_data.source)
            return;
        const auto old_source = m_data.source;
        m_data.source = source;
        emit signalImageSourceTextChanged(old_source, m_data.source);
    });
    connect(&ConfigManager::getInstance(), &ConfigManager::signalCameraConfigDataChanged, this, [&] {
        const auto &data_list = ConfigManager::getInstance().getCameraConfigData();
        const auto &info = ui->image_source_combox->currentData(Qt::UserRole).value<SourceInfo>();
        auto original_id = info.camera_id;
        ui->image_source_combox->blockSignals(true);
        ui->image_source_combox->clear();
        foreach (const auto &data, data_list)
        {
            auto status = vtk::common::CameraStatusGetter::getInstance().signalGetCameraStatus(data.name);
            QString icon_url;
            switch (status)
            {
                case vtk::common::CameraState::closed:
                    icon_url = QString(":/uiframe/images/viewtoolbar/camera_off.svg");
                    break;
                case vtk::common::CameraState::opening:
                    icon_url = QString(":/uiframe/images/viewtoolbar/camera_on.svg");
                    break;
                case vtk::common::CameraState::disconnect:
                    icon_url = QString(":/uiframe/images/viewtoolbar/camera_disconnect.svg");
                    break;
            }
            SourceInfo info = {data.id, status};
            ui->image_source_combox->addItem(QIcon(icon_url), data.name, QVariant::fromValue(info));
        }

        bool find = false;
        for (int i = 0; i < ui->image_source_combox->count(); i++)
        {
            const auto &info = ui->image_source_combox->itemData(i).value<SourceInfo>();
            if (original_id == info.camera_id)
            {
                ui->image_source_combox->setCurrentIndex(i);
                find = true;
                break;
            }
        }
        if (!find)
            ui->image_source_combox->setCurrentIndex(0);
        ui->image_source_combox->blockSignals(false);
        m_data.source = ui->image_source_combox->currentText();
    });
    connect(ui->trigger_character_lineedit, &QLineEdit::editingFinished, this, [&] { disableFocus(2); });
    connect(ui->trigger_level_combox, &QComboBox::currentTextChanged, this, [&](const QString &trigger_level) {
        if (m_data.trigger_level == trigger_level)
            return;

        m_data.trigger_level = trigger_level;
        emit signalTriggerLevelTextChanged(m_data.source, trigger_level);
    });
    connect(ui->trigger_level_order_combox, &QComboBox::currentTextChanged, this, [&](const QString &order) {
        if (m_data.trigger_level_order == order)
            return;

        const auto old_text = m_data.trigger_level_order;
        m_data.trigger_level_order = order;
        emit signalTriggerLevelOrderTextChanged(m_data.source, old_text, order);
    });
    connect(ui->image_source_combox, SIGNAL(activated(int)), this, SLOT(slotCheckCameraStatus(int)));

    connect(&ConfigManager::getInstance(), &ConfigManager::signalTriggerModeChanged, this, [&] {
        ui->trigger_level_combox->setCurrentIndex(0);
        ui->trigger_level_combox->setEnabled(ConfigManager::getInstance().getTriggerMode() == TriggerMode::softTrigger);
    });
}

void ImageSourceSettingWidget::on_delete_btn_clicked()
{
    emit signalDelete();
}

void ImageSourceSettingWidget::slotCheckCameraStatus(int index)
{
    if (index >= 0)
    {
        const auto &info = ui->image_source_combox->itemData(index).value<SourceInfo>();
        if (info.state == vtk::common::CameraState::disconnect)
        {
            QString msg = QString(tr("Camera of \"%1\" is disconnected, please ensure if continue to use that camera."))
                              .arg(ui->image_source_combox->currentText());
            MessageDialog message_box_dialog(DialogType::WarningDialog, msg);
            message_box_dialog.exec();
        }
        else if (info.state == vtk::common::CameraState::closed)
        {
            QString msg = QString(tr("Camera of \"%1\" isn't opened, please ensure if continue to use that camera."))
                              .arg(ui->image_source_combox->currentText());
            MessageDialog message_box_dialog(DialogType::WarningDialog, msg);
            message_box_dialog.exec();
        }
    }
}

}  // namespace vtk::display
