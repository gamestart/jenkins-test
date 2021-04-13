/****************************************************************************
 *  @file     tablewidget.h
 *  @brief    通用表格部件类
 *  @author   junjie.zeng
 *  @email    junjie.zeng@smartmore.com
 *  @version
 *  @date     2020.12.21
 *  Copyright (c) 2020 SmartMore All rights reserved.
 ****************************************************************************/
#ifndef TABLEWIDGET_H
#define TABLEWIDGET_H

#include "tablemodel.h"
#include <QWidget>
#include <vector>

namespace Ui
{
class TableWidget;
}

namespace vtk::display
{

class TableWidget : public QWidget
{
    Q_OBJECT

public:
    struct HeadData
    {
        QString name;
        size_t colWidth{60};
    };
    explicit TableWidget(QWidget *parent = nullptr);
    ~TableWidget();

    void setTitle(const QString &title);
    void resetCombox(const QStringList &filter_list);

    void initTable(const QString &title, const std::vector<HeadData> &head_data_list);
    void appendRows(const QList<QList<QVariant>> &data_lists);
    void appendRow(const QList<QVariant> &data_list);
    void deleteRow(int row);
    void deleteRows(int start_row, int end_row);
    void deleteAllRows();
    void deleteRowsByCondition(int col, const QVariant &element);
    int getTableRowCount() const;
    int getShownTableRowCount() const;
    void setFilter(int col = 0, const QVariant &filter = QVariant());
    void setData(int row = 0, int col = 0, const QVariant &value = QVariant());
    void setColumnHidden(int column, bool hide);
    void exportModeDatas();
    void setHeader(const QStringList &);
    int getImageRow(const QUuid &);
    bool isRowHidden(int row);

signals:
    void signalSendSelectRowData(const QList<QVariant> &row_data);
    void signalComboxTextChange(const QString &text);

protected:
    void changeEvent(QEvent *e) override;

private slots:
    void on_export_btn_clicked();
    void slotShowToolTip(QModelIndex);

private:
    void init();
    void initWidgets();
    void initConnects();

private:
    Ui::TableWidget *ui;
    TableModel *m_model{nullptr};
};
}  // namespace vtk::display
#endif  // TABLEWIDGET_H
