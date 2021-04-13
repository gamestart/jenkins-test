/****************************************************************************
 *  @file     waitingdialog.h
 *  @brief    通用等待窗口类
 *  @author   ZHU Leyan
 *  @email
 *  @version
 *  @date     2021.02.05
 *  Copyright (c) 2020 SmartMore All rights reserved.
 ****************************************************************************/
#ifndef WAITINGDIALOG_H
#define WAITINGDIALOG_H

#include <QDialog>
#include <QWidget>

namespace Ui
{
class WaitingDialog;
}

namespace vtk::display
{

class WaitingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit WaitingDialog(QWidget *parent = nullptr);
    ~WaitingDialog();

signals:
    void signalShowFinished();

protected:
    void showEvent(QShowEvent *event) override;

private:
    Ui::WaitingDialog *ui;

private slots:
    void slotClose();
};

}  // namespace vtk::display
#endif  // WAITINGDIALOG_H
