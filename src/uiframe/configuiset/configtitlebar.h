/****************************************************************************
 *  @file     configtitlebar.h
 *  @brief    设置窗口标题栏类
 *  @author
 *  @email
 *  @version
 *  @date     2020.12.21
 *  Copyright (c) 2020 SmartMore All rights reserved.
 ****************************************************************************/
#pragma once
#include <QLabel>
#include <QPushButton>
#include <QTimer>
#include <QWidget>

namespace vtk::display
{

class ConfigTitleBar : public QWidget
{
    Q_OBJECT
public:
    explicit ConfigTitleBar(QWidget *parent = 0);

    //设置标题栏背景色
    void setBackgroundColor(int r, int g, int b);
    //设置标题栏图标
    void setTitleIcon(QString filePath, QSize IconSize = QSize(25, 25));
    //设置标题栏内容
    void setTitleContent(QString titleContent);
    //设置标题栏长度
    void setTitleWidth(int width);
    // 设置窗口边框宽度;
    void setWindowBorderWidth(int borderWidth);

signals:
    void signalClose();

private:
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

    //控件初始化
    void init();

private:
    QLabel *m_icon{nullptr};   // 标题栏图标;
    QLabel *m_title{nullptr};  // 标题栏内容;
    QWidget *m_parent{nullptr};

    // 标题栏背景色;
    int m_color_r;
    int m_color_g;
    int m_color_b;

    // 移动窗口的变量;
    bool m_is_pressed;
    QPoint m_start_move_pos;

    // 标题栏内容;
    QString m_title_content;

    // 窗口边框宽度;
    int m_window_border_width;
};
}  // namespace vtk::display
