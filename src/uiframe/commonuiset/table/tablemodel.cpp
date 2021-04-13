/****************************************************************************
 *  @file     tablemodel.cpp
 *  @brief    表格模型类
 *  @author   junjie.zeng
 *  @email    junjie.zeng@smartmore.com
 *  @version
 *  @date     2020.12.25
 *  Copyright (c) 2020 SmartMore All rights reserved.
 ****************************************************************************/
#include "tablemodel.h"

namespace vtk::display
{

TableModel::TableModel(const QStringList &head_list, QObject *parent)
    : QAbstractTableModel(parent), m_head_list(head_list)
{
}

int TableModel::rowCount(const QModelIndex &parent) const
{
    (void)parent;
    return m_model_data.count();
}

int TableModel::columnCount(const QModelIndex &parent) const
{
    (void)parent;
    return m_head_list.count();
}

QVariant TableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (section >= 0 && section < columnCount())
    {
        if (orientation == Qt::Horizontal)
        {
            if (role == Qt::DisplayRole)
                return m_head_list[section];
        }
    }
    return QVariant();
}

QVariant TableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    auto row = index.row();
    auto col = index.column();
    if (row >= rowCount() || row < 0 || col >= columnCount() || col < 0)
        return QVariant();

    if (role == Qt::DisplayRole)
    {
        auto item_data = m_model_data.at(row).at(col);
        return item_data;
    }

    return QVariant();
}

bool TableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.isValid() && role == Qt::EditRole)
    {
        const auto row = index.row();
        const auto col = index.column();
        if (row >= rowCount() || row < 0 || col >= columnCount() || col < 0)
            return false;

        m_model_data.operator[](row).operator[](col) = value;
        emit dataChanged(index, index);
        return true;
    }
    return false;
}

void TableModel::appendRows(const QList<QList<QVariant>> &data_lists)
{
    auto count = rowCount();
    auto rows = data_lists.count();
    if (rows > 0)
    {
        beginInsertRows(QModelIndex(), count, count + rows - 1);
        m_model_data.append(data_lists);
        endInsertRows();
    }
}

void TableModel::appendRow(const QList<QVariant> &data_list)
{
    auto count = rowCount();
    beginInsertRows(QModelIndex(), count, count);
    m_model_data.append(data_list);
    endInsertRows();
}

void TableModel::deleteRow(int row)
{
    auto count = rowCount();
    if (count > 0 && row >= 0 && row < count)
    {
        beginRemoveRows(QModelIndex(), row, row);
        m_model_data.removeAt(row);
        endRemoveRows();
    }
}

void TableModel::deleteRows(int start_row, int end_row)
{
    auto count = rowCount();
    if (count > 0 && start_row <= end_row && start_row > 0 && end_row < count)
    {
        beginRemoveRows(QModelIndex(), start_row, end_row);
        for (int i = 0; i <= end_row - start_row; i++)
            m_model_data.removeAt(start_row);
        endRemoveRows();
    }
}

void TableModel::deleteAllRows()
{
    auto count = rowCount();
    if (count > 0)
    {
        beginRemoveRows(QModelIndex(), 0, count - 1);
        m_model_data.clear();
        endRemoveRows();
    }
}

void TableModel::deleteRowsByCondition(int col, QVariant element)
{
    for (int row = 0; row < m_model_data.size();)
    {
        if (m_model_data.at(row).at(col) == element)
            deleteRow(row);
        else
            row++;
    }
}

std::vector<int> TableModel::containElementInRow(int col, QVariant element)
{
    std::vector<int> rows;
    for (int row = 0; row < m_model_data.size(); row++)
    {
        if (m_model_data.at(row).at(col) == element)
            rows.push_back(row);
    }
    return rows;
}

QList<QVariant> TableModel::getRowData(int row) const
{
    auto count = rowCount();
    return (count > row && row >= 0) ? m_model_data.at(row) : QList<QVariant>();
}

const QStringList &TableModel::getHeadList() const
{
    return m_head_list;
}

void TableModel::setHeadList(const QStringList &header_list)
{
    m_head_list = header_list;
}

}  // namespace vtk::display
