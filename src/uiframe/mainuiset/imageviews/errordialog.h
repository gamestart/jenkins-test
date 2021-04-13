/****************************************************************************
 *  @file     errordialog.h
 *  @brief    主界面相机断开连接后显示的错误信息窗口类
 *  @author   ZHU Leyan
 *  @email
 *  @version
 *  @date     2021.03.05
 *  Copyright (c) 2020 SmartMore All rights reserved.
 ****************************************************************************/
#ifndef ERRORDIALOG_H
#define ERRORDIALOG_H

#include <QWidget>

namespace Ui
{
class ErrorDialog;
}

namespace vtk::display
{
class ErrorDialog : public QWidget
{
    Q_OBJECT

public:
    explicit ErrorDialog(QWidget *parent = nullptr);
    ~ErrorDialog();
    void updateDisconnectSecs();
    void resetDisconnectSecs();

signals:
    void signalCloseErrorDialog();

protected:
    void changeEvent(QEvent *e) override;

private slots:
    void on_close_btn_clicked();

private:
    Ui::ErrorDialog *ui;
    int m_secs{0};
};
}  // namespace vtk::display
#endif  // ERRORDIALOG_H
