/****************************************************************************
 *  @file     aboutdialog.h
 *  @brief    关于页面类
 *  @author   junjie.zeng
 *  @email    junjie.zeng@smartmore.com
 *  @version
 *  @date     2020.12.21
 *  Copyright (c) 2020 SmartMore All rights reserved.
 ****************************************************************************/
#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QDialog>

namespace Ui
{
class AboutDialog;
}

namespace vtk::display
{

class AboutDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AboutDialog(QWidget *parent = nullptr);
    ~AboutDialog();

private slots:
    void on_close_btn_clicked();
    void on_website_btn_clicked();

private:
    void init();
    void initWidgets();

private:
    Ui::AboutDialog *ui;
};
}  // namespace vtk::display
#endif  // ABOUTDIALOG_H
