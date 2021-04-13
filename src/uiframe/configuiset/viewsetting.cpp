/****************************************************************************
 *  @file     viewsetting.cpp
 *  @brief    设置窗口单个显示设置窗口类
 *  @author
 *  @email
 *  @version
 *  @date     2020.12.21
 *  Copyright (c) 2020 SmartMore All rights reserved.
 ****************************************************************************/
#include <QListView>
#include <QMetaType>

#include "aialgoservice/aialgomanager.h"
#include "camerasetting.h"
#include "configdatamanager/configmanager.h"
#include "configdialog.h"
#include "modelitem.h"
#include "uiframe/commonuiset/messagedialog/messagedialog.h"
#include "viewsetting.h"

namespace vtk::display
{
using namespace vtk::common;
using ConfigManager = vtk::config::ConfigManager;
using AlgoConfigData = vtk::config::AlgoConfigData;
using SourceConfigData = vtk::config::SourceConfigData;

ViewSetting::ViewSetting(const QUuid &view_uuid, const QString &view_name, const QString &view_source,
                         const QStringList &view_algo, QWidget *parent)
    : QWidget(parent), ui(new Ui::ViewSetting), m_uuid(view_uuid)
{
    initWidgets(view_name, view_source, view_algo);
    initConnections();
}

ViewSetting::~ViewSetting()
{
    delete ui;
}

void ViewSetting::initWidgets(const QString &view_name, const QString &view_source, const QStringList &view_algo)
{
    ui->setupUi(this);

    ui->delete_model_item_btn->setEnabled(false);

    ui->view_source_list->setView(new QListView(ui->view_source_list));
    ui->view_name_input->setText(view_name);
    ui->view_name->setText(view_name);

    ui->view_source_list->addItem("File");
    updateViewSources();
    ui->view_source_list->setCurrentText(view_source);

    connect(ui->model_items_widget, SIGNAL(currentRowChanged(int)), this, SLOT(slotChangeDeleteModelBtnStatus()));
    initModelListWidget(view_algo);

    ui->view_name_input->installEventFilter(this);
    ui->view_source_list->installEventFilter(this);
}

void ViewSetting::initConnections()
{
    connect(ui->delete_btn, SIGNAL(clicked()), this, SLOT(slotDeleteView()));
    connect(ui->view_name_input, &QLineEdit::editingFinished, this, [=] { disableFocus(); });
    connect(&ConfigManager::getInstance(), &ConfigManager::signalAlgoConfigDataChanged, this, [&] {
        for (int i = 0; i < ui->model_items_widget->count(); i++)
        {
            auto model_item = ui->model_items_widget->item(i);
            auto model_item_widget = dynamic_cast<ModelItem *>(ui->model_items_widget->itemWidget(model_item));
            model_item_widget->resetList();
            if (is_deleted)
                i--;
            is_deleted = false;
        }
        for (int i = 0; i < ui->model_items_widget->count(); i++)
        {
            auto model_item = ui->model_items_widget->item(i);
            auto model_item_widget = dynamic_cast<ModelItem *>(ui->model_items_widget->itemWidget(model_item));
            model_item_widget->changeItemStatus();
        }
    });
    connect(&ConfigManager::getInstance(), &ConfigManager::signalSourceConfigDataChanged, this, [&] {
        SourceConfigData source_data = ConfigManager::getInstance().getSourceConfigData();
        QUuid original_id = ui->view_source_list->currentData().toUuid();
        ui->view_source_list->clear();
        ui->view_source_list->addItem("File");
        int source_index = 0;
        int count = ConfigManager::getInstance().getViewConfigData().size();
        for (const auto &source : source_data)
        {
            if (source_index >= count)
            {
                QVariant var;
                var.setValue(source.uuid);
                ui->view_source_list->addItem(source.source_name, var);
            }
            source_index++;
        }
        bool find = false;
        for (int i = 0; i < ui->view_source_list->count(); i++)
        {
            if (original_id == ui->view_source_list->itemData(i).toUuid())
            {
                ui->view_source_list->setCurrentIndex(i);
                find = true;
                break;
            }
        }
        if (!find)
            ui->view_source_list->setCurrentIndex(0);
    });
}

void ViewSetting::initModelListWidget(QStringList view_algos)
{
    for (const auto view_algo : view_algos)
    {
        addOneModelItem(view_algo);
    }
}

void ViewSetting::slotChangeDeleteModelBtnStatus()
{
    if (ui->model_items_widget->currentRow() < 0)
        ui->delete_model_item_btn->setEnabled(false);
    else
        ui->delete_model_item_btn->setEnabled(true);
}

void ViewSetting::on_add_model_item_btn_clicked()
{
    addOneModelItem("");
}

void ViewSetting::on_delete_model_item_btn_clicked()
{
    auto item = ui->model_items_widget->currentItem();
    auto item_widget = dynamic_cast<ModelItem *>(ui->model_items_widget->itemWidget(item));
    ui->model_items_widget->removeItemWidget(item);
    if (item)
        delete item;
    if (item_widget)
        delete item_widget;
    for (int i = 0; i < ui->model_items_widget->count(); i++)
    {
        auto model_item = ui->model_items_widget->item(i);
        auto model_item_widget = dynamic_cast<ModelItem *>(ui->model_items_widget->itemWidget(model_item));
        model_item_widget->setId(i + 1);
        model_item_widget->changeItemStatus();
    }
}

void ViewSetting::addOneModelItem(QString model_name)
{
    auto item = new QListWidgetItem(ui->model_items_widget);
    auto widget = new ModelItem(model_name, ui->model_items_widget);
    item->setSizeHint(QSize(240, 34));
    ui->model_items_widget->addItem(item);
    ui->model_items_widget->setItemWidget(item, widget);
    auto id = ui->model_items_widget->row(item) + 1;
    widget->setId(id);
    connect(widget, &ModelItem::signalNameSelected, this, [&](const int &id) {
        for (int i = 0; i < ui->model_items_widget->count(); i++)
        {
            if (i + 1 == id)
                continue;
            auto model_item = ui->model_items_widget->item(i);
            auto model_item_widget = dynamic_cast<ModelItem *>(ui->model_items_widget->itemWidget(model_item));
            model_item_widget->changeItemStatus();
        }
    });
    connect(widget, &ModelItem::signalIsNameUsed, this, [&](const QString &name) {
        for (int i = 0; i < ui->model_items_widget->count(); i++)
        {
            auto model_item = ui->model_items_widget->item(i);
            auto model_item_widget = dynamic_cast<ModelItem *>(ui->model_items_widget->itemWidget(model_item));
            if (name == model_item_widget->getText())
                return true;
        }
        return false;
    });
    connect(widget, &ModelItem::signalDelete, this, [=] {
        ui->model_items_widget->removeItemWidget(item);
        delete item;
        delete widget;
        is_deleted = true;
        for (int i = 0; i < ui->model_items_widget->count(); i++)
        {
            auto model_item = ui->model_items_widget->item(i);
            auto model_item_widget = dynamic_cast<ModelItem *>(ui->model_items_widget->itemWidget(model_item));
            model_item_widget->setId(i + 1);
        }
    });
    widget->changeItemStatus();
}

QUuid ViewSetting::getUuid()
{
    return m_uuid;
}

QString ViewSetting::getViewName()
{
    return ui->view_name_input->text();
}

QString ViewSetting::getViewSource()
{
    return ui->view_source_list->currentText();
}

QStringList ViewSetting::getViewAlgo()
{
    QStringList algo_list;
    for (int i = 0; i < ui->model_items_widget->count(); i++)
    {
        auto model_item = ui->model_items_widget->item(i);
        auto model_item_widget = dynamic_cast<ModelItem *>(ui->model_items_widget->itemWidget(model_item));
        const auto &algo = model_item_widget->getText();
        if (!algo.isEmpty())
            algo_list.push_back(algo);
    }
    return algo_list;
}

void ViewSetting::setViewName(QString view_name)
{
    ui->view_name_input->setText(view_name);
    ui->view_name->setText(view_name);
}

void ViewSetting::slotDeleteView()
{
    if (ui->delete_btn->property("status") == "enabled")
    {
        emit signalDeleteView();
    }
    else
    {
        MessageDialog at_least_one_view_dialog(DialogType::WarningDialog, tr("Please keep one view at least"));
        at_least_one_view_dialog.exec();
    }
}

void ViewSetting::disableFocus()
{
    if (validateViewName())
        ui->view_name->setText(ui->view_name_input->text());
    ui->view_name_input->clearFocus();
}

void ViewSetting::enableDeleteBtn(bool enable)
{
    if (enable)
    {
        ui->delete_btn->setProperty("status", "enabled");
        ui->delete_btn->setCursor(Qt::PointingHandCursor);
    }
    else
    {
        ui->delete_btn->setProperty("status", "disabled");
        ui->delete_btn->setCursor(Qt::ForbiddenCursor);
    }
}

bool ViewSetting::validateViewSetting()
{
    int flag = 0;
    if (!validateViewName())
        flag++;
    if (!validateViewSource())
        flag++;
    if (flag)
        return false;
    else
        return true;
}

bool ViewSetting::validateViewSource()
{
    if (ui->view_source_list->currentText().isEmpty())
    {
        ui->view_source_warning->setText(tr("Please select the input list."));
        return false;
    }
    ui->view_source_warning->setText("");
    return true;
}

bool ViewSetting::validateViewName()
{
    if (ui->view_name_input->isEmpty())
    {
        ui->view_name_warning->setText(tr("Please enter the view name."));
        return false;
    }
    if (ui->view_name_input->containsSpecial())
    {
        ui->view_name_warning->setText(tr("No special character please."));
        return false;
    }
    if (ui->view_name_input->containsSpace())
    {
        ui->view_name_warning->setText(tr("No space character please."));
        return false;
    }
    if (ui->view_name_input->exceedLength(10))
    {
        ui->view_name_warning->setText(tr("The name length is between 1 and 10 characters."));
        return false;
    }

    bool is_duplicate = emit signalCheckDuplicate();
    if (is_duplicate)
    {
        ui->view_name_warning->setText(tr("View name can't be duplicate."));
        return false;
    }

    ui->view_name_warning->setText("");
    return true;
}

bool ViewSetting::eventFilter(QObject *wcg, QEvent *event)
{
    if (event->type() == QEvent::Wheel)
        return true;
    if (event->type() == QEvent::FocusIn && wcg == ui->view_name_input)
    {
        ui->view_name_warning->setText("");
    }
    return QWidget::eventFilter(wcg, event);
}

void ViewSetting::updateViewModels()
{
    for (int i = 0; i < ui->model_items_widget->count(); i++)
    {
        auto model_item = ui->model_items_widget->item(i);
        auto model_item_widget = dynamic_cast<ModelItem *>(ui->model_items_widget->itemWidget(model_item));
        model_item_widget->resetList();
        if (is_deleted)
            i--;
        is_deleted = false;
    }
    for (int i = 0; i < ui->model_items_widget->count(); i++)
    {
        auto model_item = ui->model_items_widget->item(i);
        auto model_item_widget = dynamic_cast<ModelItem *>(ui->model_items_widget->itemWidget(model_item));
        model_item_widget->changeItemStatus();
    }
}

void ViewSetting::updateViewSources()
{
    SourceConfigData source_data = ConfigManager::getInstance().getSourceConfigData();
    QUuid original_id = ui->view_source_list->currentData().toUuid();
    ui->view_source_list->clear();
    ui->view_source_list->addItem("File");
    int source_index = 0;
    int count = ConfigManager::getInstance().getViewConfigData().size();
    for (const auto &source : source_data)
    {
        if (source_index >= count)
        {
            QVariant var;
            var.setValue(source.uuid);
            ui->view_source_list->addItem(source.source_name, var);
        }
        source_index++;
    }
    bool find = false;
    for (int i = 0; i < ui->view_source_list->count(); i++)
    {
        if (original_id == ui->view_source_list->itemData(i).toUuid())
        {
            ui->view_source_list->setCurrentIndex(i);
            find = true;
            break;
        }
    }
    if (!find)
        ui->view_source_list->setCurrentIndex(0);
}

}  // namespace vtk::display
