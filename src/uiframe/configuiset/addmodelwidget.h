/****************************************************************************
 *  @file     addmodelwidget.h
 *  @brief    设置窗口添加模型窗口类
 *  @author
 *  @email
 *  @version
 *  @date     2021.01.07
 *  Copyright (c) 2020 SmartMore All rights reserved.
 ****************************************************************************/
#ifndef ADDMODELWIDGET_H
#define ADDMODELWIDGET_H

#include <QWidget>

namespace Ui
{
class AddModelWidget;
}

namespace vtk::display
{

class AddModelWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AddModelWidget(QWidget *parent = nullptr);
    ~AddModelWidget();

signals:
    void signalAddModel();

private slots:
    void on_add_model_btn_clicked();

private:
    Ui::AddModelWidget *ui;
};
}  // namespace vtk::display
#endif  // ADDMODELWIDGET_H
