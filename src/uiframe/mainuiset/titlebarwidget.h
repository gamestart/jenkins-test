/****************************************************************************
 *  @file     titlebarwidget.h
 *  @brief    标题栏类
 *  @author   junjie.zeng
 *  @email    junjie.zeng@smartmore.com
 *  @version
 *  @date     2020.12.21
 *  Copyright (c) 2020 SmartMore All rights reserved.
 ****************************************************************************/
#ifndef TITLEBARWIDGET_H
#define TITLEBARWIDGET_H

#include "runtimecontroller/runtimecontroller.h"
#include <QPoint>
#include <QString>
#include <QWidget>

namespace Ui
{
class TitleBarWidget;
}

namespace vtk::display
{

class TitleBarWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TitleBarWidget(QWidget *parent = nullptr);
    explicit TitleBarWidget(const QString &project_name = "", const QString &project_detail_name = "",
                            const QString &logo_path = "", QWidget *parent = nullptr);
    ~TitleBarWidget();

signals:
    void signalChangeLang();

public slots:
    void slotRunStatusChange(vtk::control::RunStatus status);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void changeEvent(QEvent *e) override;

private slots:
    void on_settings_btn_clicked();
    void on_lang_btn_clicked();
    void on_about_btn_clicked();
    void on_minimum_btn_clicked();
    void on_maximum_btn_clicked(bool checked);
    void on_close_btn_clicked();

private:
    void init();
    void initWidgets();
    void initConnects();
    void updateTitle();

private:
    Ui::TitleBarWidget *ui;
    QString m_project_name;
    QString m_project_detail_name;
    QString m_logo_path;
    QWidget *m_parent{nullptr};
    QPoint m_start_move_pos;
    QRect m_parent_last_rect;
};
}  // namespace vtk::display
#endif  // TITLEBARWIDGET_H
