/****************************************************************************
 *  @file     tablemodel.h
 *  @brief    表格模型类
 *  @author   junjie.zeng
 *  @email    junjie.zeng@smartmore.com
 *  @version
 *  @date     2020.12.25
 *  Copyright (c) 2020 SmartMore All rights reserved.
 ****************************************************************************/
#ifndef TABLEMODEL_H
#define TABLEMODEL_H
#include <QAbstractTableModel>
#include <QVariant>

namespace vtk::display
{

class TableModel : public QAbstractTableModel
{
public:
    TableModel(const QStringList &head_list, QObject *parent = nullptr);
    ~TableModel() = default;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

    void appendRows(const QList<QList<QVariant>> &data_lists);
    void appendRow(const QList<QVariant> &data_list);
    void deleteRow(int row);
    void deleteRows(int start_row, int end_row);
    void deleteAllRows();
    void deleteRowsByCondition(int col, QVariant element);
    std::vector<int> containElementInRow(int col, QVariant element);

    QList<QVariant> getRowData(int row) const;
    const QStringList &getHeadList() const;
    void setHeadList(const QStringList &);

private:
    QStringList m_head_list;
    QList<QList<QVariant>> m_model_data;
};
}  // namespace vtk::display
#endif  // TABLEMODEL_H
