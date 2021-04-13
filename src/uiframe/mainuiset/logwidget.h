/****************************************************************************
 *  @file     logwidget.h
 *  @brief    日志列表类
 *  @author   junjie.zeng
 *  @email    junjie.zeng@smartmore.com
 *  @version
 *  @date     2020.12.21
 *  Copyright (c) 2020 SmartMore All rights reserved.
 ****************************************************************************/
#ifndef LOGWIDGET_H
#define LOGWIDGET_H

#include <QWidget>

namespace Ui
{
class LogWidget;
}

namespace vtk::display
{

class LogWidget : public QWidget
{
    Q_OBJECT

public:
    explicit LogWidget(const QString &title = "", QWidget *parent = nullptr);
    ~LogWidget();
    void setTitle(QString);
    void setHeader(QStringList);

private:
    void init();
    void initWidgets();
    void initConnects();

private:
    Ui::LogWidget *ui;
    QString m_title;
};
}  // namespace vtk::display
#endif  // LOGWIDGET_H
