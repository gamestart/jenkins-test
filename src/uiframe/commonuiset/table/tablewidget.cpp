/****************************************************************************
 *  @file     tablewidget.cpp
 *  @brief    通用表格部件类
 *  @author   junjie.zeng
 *  @email    junjie.zeng@smartmore.com
 *  @version
 *  @date     2020.12.21
 *  Copyright (c) 2020 SmartMore All rights reserved.
 ****************************************************************************/
#include "tablewidget.h"
#include "ui_tablewidget.h"
#include <QDateTime>
#include <QDebug>
#include <QFileDialog>
#include <QListView>
#include <QTextStream>
#include <QToolTip>

namespace vtk::display
{

TableWidget::TableWidget(QWidget *parent) : QWidget(parent), ui(new Ui::TableWidget)
{
    init();
}

TableWidget::~TableWidget()
{
    delete ui;
}

void TableWidget::initTable(const QString &title, const std::vector<HeadData> &head_data_list)
{
    setTitle(title);
    QStringList head_name_list;
    for (const auto &head_item : head_data_list)
        head_name_list.append(head_item.name);

    m_model = new TableModel(head_name_list, ui->table_view);
    ui->table_view->setModel(m_model);
    for (int i = 0; i < head_data_list.size(); i++)
        ui->table_view->setColumnWidth(i, head_data_list.at(i).colWidth);
    ui->table_view->hide();
    ui->no_data_widget->show();
    ui->no_data_layout->setAlignment(Qt::AlignCenter);
    connect(ui->table_view->selectionModel(), &QItemSelectionModel::currentRowChanged, this,
            [&](const QModelIndex &current_index) {
                emit signalSendSelectRowData(m_model->getRowData(current_index.row()));
            });
}

void TableWidget::appendRows(const QList<QList<QVariant>> &data_lists)
{
    m_model->appendRows(data_lists);
    ui->table_view->show();
    ui->no_data_widget->hide();
}

void TableWidget::appendRow(const QList<QVariant> &data_list)
{
    m_model->appendRow(data_list);
    ui->table_view->show();
    ui->no_data_widget->hide();
}

void TableWidget::deleteRow(int row)
{
    m_model->deleteRow(row);
    if (!m_model->rowCount())
    {
        ui->table_view->hide();
        ui->no_data_widget->show();
    }
}

void TableWidget::deleteRows(int start_row, int end_row)
{
    m_model->deleteRows(start_row, end_row);
    if (!m_model->rowCount())
    {
        ui->table_view->hide();
        ui->no_data_widget->show();
    }
}

void TableWidget::deleteAllRows()
{
    m_model->deleteAllRows();
    if (!m_model->rowCount())
    {
        ui->table_view->hide();
        ui->no_data_widget->show();
    }
}

void TableWidget::deleteRowsByCondition(int col, const QVariant &element)
{
    m_model->deleteRowsByCondition(col, element);
    if (!m_model->rowCount())
    {
        ui->table_view->hide();
        ui->no_data_widget->show();
    }
}

int TableWidget::getShownTableRowCount() const
{
    int count = 0;
    for (int i = 0; i < m_model->rowCount(); i++)
    {
        if (!ui->table_view->isRowHidden(i))
            count++;
    }
    return count;
}

int TableWidget::getTableRowCount() const
{
    return m_model->rowCount();
}

bool TableWidget::isRowHidden(int row)
{
    return ui->table_view->isRowHidden(row);
}

void TableWidget::setFilter(int col, const QVariant &filter)
{
    if (filter.isNull())  //显示所有行
    {
        for (int i = 0; i < m_model->rowCount(); i++)
            ui->table_view->setRowHidden(i, false);
        if (m_model->rowCount())
        {
            ui->table_view->show();
            ui->no_data_widget->hide();
        }
        else
        {
            ui->no_data_widget->show();
            ui->table_view->hide();
        }
        return;
    }

    auto rows = m_model->containElementInRow(col, filter);
    for (int row = 0; row < m_model->rowCount(); row++)
        ui->table_view->setRowHidden(row, std::count(rows.begin(), rows.end(), row) == 0);
    if (!rows.size())
    {
        ui->no_data_widget->show();
        ui->table_view->hide();
    }
    else
    {
        ui->table_view->show();
        ui->no_data_widget->hide();
    }
}

void TableWidget::setData(int row, int col, const QVariant &value)
{
    auto index = m_model->index(row, col);
    m_model->setData(index, value);
}

void TableWidget::setColumnHidden(int column, bool hide)
{
    ui->table_view->setColumnHidden(column, hide);
}

void TableWidget::exportModeDatas()
{
    QVariant filter = ui->filter_combox->currentText();

    QList<int> display_col_list;
    const auto &head_list = m_model->getHeadList();
    const auto column_count = m_model->columnCount();
    for (int col = 0; col < column_count; col++)
    {
        if (!ui->table_view->isColumnHidden(col))
            display_col_list.append(col);
    }

    auto file_name = QDateTime::currentDateTime().toString("yyyy-mm-dd-hh-mm-ss-zzz");
    file_name += "_List_" + filter.toString();
    auto file_path = QFileDialog::getSaveFileName(nullptr, "Export data", "./" + file_name, "CSV Files (*.csv)");
    QFile file(file_path);
    if (file.open(QIODevice::WriteOnly))
    {
        QTextStream stream(&file);
#ifdef Q_OS_WIN
        stream.setCodec("GBK");
#else
        stream.setCodec("UTF-8");
#endif
        for (int col = 0; col < column_count; col++)
        {
            if (display_col_list.contains(col))
            {
                if (display_col_list.last() != col)
                    stream << head_list.at(col) << ",";
                else
                    stream << head_list.at(col) << endl;
            }
        }

        for (int row = 0; row < m_model->rowCount(); row++)
        {
            if (ui->table_view->isRowHidden(row))
                continue;

            auto row_data = m_model->getRowData(row);
            for (int col = 0; col < column_count; col++)
            {
                if (display_col_list.contains(col))
                {
                    if (display_col_list.last() != col)
                        stream << row_data.at(col).toString() << ",";
                    else
                        stream << row_data.at(col).toString() << endl;
                }
            }
        }

        stream.flush();
        file.close();
    }
}

void TableWidget::init()
{
    initWidgets();
    initConnects();
}

void TableWidget::initWidgets()
{
    ui->setupUi(this);
    ui->filter_combox->setView(new QListView());
    ui->table_view->setMouseTracking(true);
}

void TableWidget::initConnects()
{
    connect(ui->table_view, &QTableView::clicked, this,
            [&](const QModelIndex &index) { emit signalSendSelectRowData(m_model->getRowData(index.row())); });
    connect(ui->filter_combox, &QComboBox::currentTextChanged, this, &TableWidget::signalComboxTextChange);
    connect(ui->table_view, SIGNAL(entered(QModelIndex)), this, SLOT(slotShowToolTip(QModelIndex)));
}

void TableWidget::setTitle(const QString &title)
{
    ui->title_lab->setText(title);
}

void TableWidget::setHeader(const QStringList &header_list)
{
    m_model->setHeadList(header_list);
    ui->table_view->setModel(m_model);
}

void TableWidget::resetCombox(const QStringList &filter_list)
{
    ui->filter_combox->blockSignals(true);
    ui->filter_combox->clear();
    foreach (const auto &filter, filter_list)
        ui->filter_combox->addItem(filter);
    ui->filter_combox->blockSignals(false);
    ui->filter_combox->setCurrentIndex(0);
    emit signalComboxTextChange(ui->filter_combox->currentText());
}

void TableWidget::on_export_btn_clicked()
{
    exportModeDatas();
}

void TableWidget::changeEvent(QEvent *e)  //重写的事件处理方法
{
    QWidget::changeEvent(e);  //让基类执行事件处理方法
    switch (e->type())
    {
        case QEvent::LanguageChange:  //如果是语言改变事件
            if (ui)
            {
                ui->retranslateUi(this);
                ui->export_btn->setText(tr("Export"));  //更新UI的语言
            }
            break;
        default:
            break;
    }
}

int TableWidget::getImageRow(const QUuid &image_uuid)
{
    for (int i = 0; i < m_model->rowCount(); i++)
        if (m_model->getRowData(i).at(6) == image_uuid)
            return i;
}

void TableWidget::slotShowToolTip(QModelIndex index)
{
    if (index.column() == 1)
    {
        QToolTip::showText(QCursor::pos(), m_model->getRowData(index.row()).at(index.column()).toString());
    }
}

}  // namespace vtk::display
