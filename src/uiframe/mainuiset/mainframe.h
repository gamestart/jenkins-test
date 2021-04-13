/****************************************************************************
 *  @file     mainframe.h
 *  @brief    界面主显示框类
 *  @author   junjie.zeng
 *  @email    junjie.zeng@smartmore.com
 *  @version
 *  @date     2020.12.21
 *  Copyright (c) 2020 SmartMore All rights reserved.
 ****************************************************************************/
#ifndef MAINFRAME_H
#define MAINFRAME_H

#include <QMainWindow>
#include <QTimer>
#include <QTranslator>

namespace Ui
{
class MainFrame;
}

namespace vtk::display
{
class TitleBarWidget;
class StatisticsWidget;
class OperationalControlWidget;
class LogWidget;
class DataListWidget;
class ErrorDialog;

class MainFrame : public QMainWindow
{
    Q_OBJECT

public:
    enum class ViewFrameType
    {
        singleView = 0,  //单视图
        doubleViews,     //双视图
        threeViews,      //三视图
        fourViews,       //四视图
        multiViews       //多视图(大于四个视图)
    };

    explicit MainFrame(QWidget *parent = nullptr);
    ~MainFrame();
    void setTranslator(QTranslator *);

private slots:
    void slotChangeLang();

private:
    void init();
    void initWidgets();
    void initConnects();
    void resetViewFrame();
    void updateViewFrame();
    void updateViewFrameType();
    void updateDataListWidget();

private:
    Ui::MainFrame *ui;
    TitleBarWidget *m_titlebar_widget{nullptr};
    StatisticsWidget *m_statistics_widget{nullptr};
    OperationalControlWidget *m_operational_control_widget{nullptr};
    LogWidget *m_log_widget{nullptr};
    DataListWidget *m_data_list_widget{nullptr};
    QWidget *m_view_frame{nullptr};
    ViewFrameType m_view_frame_type{ViewFrameType::singleView};
    // ErrorDialog *m_error_dialog{nullptr};

    int m_lang{0};
    QTranslator *m_translator{nullptr};
    QTimer *m_timer{nullptr};
};
}  // namespace vtk::display
#endif  // MAINFRAME_H
