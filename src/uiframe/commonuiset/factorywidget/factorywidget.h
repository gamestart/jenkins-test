/****************************************************************************
 *  @file     factorywidget.h
 *  @brief    小部件添加工厂类
 *  @author   junjie.zeng
 *  @email    junjie.zeng@smartmore.com
 *  @version
 *  @date     2021.01.08
 *  Copyright (c) 2021 SmartMore All rights reserved.
 ****************************************************************************/
#ifndef FACTORYWIDGET_H
#define FACTORYWIDGET_H

#include <QWidget>

namespace Ui
{
class FactoryWidget;
}

class QListWidgetItem;

namespace vtk::display
{

class FactoryWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FactoryWidget(QWidget *parent = nullptr);
    ~FactoryWidget();

    void addListItem(QListWidgetItem *item, QWidget *widget = nullptr);
    void deleteListItem(QListWidgetItem *item);

    int getItemRow(QListWidgetItem *item) const;
    int getListItemCounts() const;

    QWidget *getItemWidget(int row) const;

    void setEnabledAddButton(bool enable);
    void setAddButtonStatus(bool enable);
    QString getAddButtonStatus();
    void clear();
    void setAddButtonToolTip(QString);

signals:
    void signalAddOne();

private slots:
    void on_add_btn_clicked();

private:
    Ui::FactoryWidget *ui;
};
}  // namespace vtk::display
#endif  // FACTORYWIDGET_H
