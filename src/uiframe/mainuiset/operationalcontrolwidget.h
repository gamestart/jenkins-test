/****************************************************************************
 *  @file     operationalcontrolwidget.h
 *  @brief    运行控制窗口类
 *  @author   junjie.zeng
 *  @email    junjie.zeng@smartmore.com
 *  @version
 *  @date     2020.12.21
 *  Copyright (c) 2020 SmartMore All rights reserved.
 ****************************************************************************/
#ifndef OPERATIONALCONTROLWIDGET_H
#define OPERATIONALCONTROLWIDGET_H

#include "runtimecontroller/runtimecontroller.h"
#include <QWidget>

namespace Ui
{
class OperationalControlWidget;
}

namespace vtk::display
{

class OperationalControlWidget : public QWidget
{
    Q_OBJECT

public:
    explicit OperationalControlWidget(QWidget *parent = nullptr);
    ~OperationalControlWidget();

signals:
    void signalRunStatusChange(vtk::control::RunStatus status);

protected:
    void changeEvent(QEvent *e) override;

private slots:
    void on_run_once_btn_clicked();
    void on_run_cycle_btn_clicked();
    void on_pause_btn_clicked();

private:
    void init();
    void initWidgets();
    void initConnects();

private:
    Ui::OperationalControlWidget *ui;
};
}  // namespace vtk::display
#endif  // OPERATIONALCONTROLWIDGET_H
