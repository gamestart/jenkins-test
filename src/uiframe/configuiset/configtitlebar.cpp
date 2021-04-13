/****************************************************************************
 *  @file     configtitlebar.cpp
 *  @brief    设置窗口标题栏类
 *  @author
 *  @email
 *  @version
 *  @date     2020.12.21
 *  Copyright (c) 2020 SmartMore All rights reserved.
 ****************************************************************************/
#include "configtitlebar.h"
#include <QApplication>
#include <QDebug>
#include <QEvent>
#include <QFile>
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QPainter>
#include <QToolButton>

namespace vtk::display
{

#define BUTTON_HEIGHT 20  // 按钮高度;
#define BUTTON_WIDTH 20   // 按钮宽度;
#define TITLE_HEIGHT 30   // 标题栏高度;

ConfigTitleBar::ConfigTitleBar(QWidget *parent)
    : QWidget(parent), m_parent(parent), m_color_r(153), m_color_g(153), m_color_b(153), m_is_pressed(false),
      m_window_border_width(0)
{
    init();
}

void ConfigTitleBar::init()
{
    m_icon = new QLabel(this);
    m_title = new QLabel(this);

    m_title->setObjectName("TitleContent");
    m_title->setAlignment(Qt::AlignCenter);
    m_title->setStyleSheet("color: rgba(0,0,0,0.8); font-size: 16px;");
    m_title->setText(tr("Settings"));
    m_title->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    QToolButton *close_button = new QToolButton;
    close_button->setFixedSize(16, 16);
    close_button->setStyleSheet("border: 0px; background: url(\":/uiframe/images/config/close.svg\");");
    close_button->setCursor(Qt::PointingHandCursor);
    connect(close_button, &QToolButton::clicked, this, &ConfigTitleBar::signalClose);

    QHBoxLayout *mylayout = new QHBoxLayout(this);
    mylayout->addWidget(m_title);
    mylayout->addWidget(close_button);
    mylayout->setSpacing(0);
    mylayout->setContentsMargins(26, 0, 10, 0);

    this->setWindowFlags(Qt::FramelessWindowHint);

    setBackgroundColor(233, 238, 242);
}

void ConfigTitleBar::setBackgroundColor(int r, int g, int b)
{
    // 标题栏背景色;
    m_color_r = r;
    m_color_g = g;
    m_color_b = b;

    update();
}

void ConfigTitleBar::setTitleIcon(QString filePath, QSize IconSize)
{
    QPixmap titleIcon(filePath);
    m_icon->setPixmap(titleIcon.scaled(IconSize));
}

void ConfigTitleBar::setTitleContent(QString titleContent)
{
    m_title->setText(titleContent);
    m_title_content = titleContent;
}

void ConfigTitleBar::setTitleWidth(int width)
{
    this->setFixedWidth(width);
}

void ConfigTitleBar::setWindowBorderWidth(int borderWidth)
{
    m_window_border_width = borderWidth;
}

void ConfigTitleBar::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    QPainterPath pathBack;
    pathBack.setFillRule(Qt::WindingFill);
    pathBack.addRoundedRect(QRect(0, 0, this->width(), this->height()), 3, 3);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    painter.fillPath(pathBack, QBrush(QColor(m_color_r, m_color_g, m_color_b)));
    if (this->width() != (this->parentWidget()->width() - m_window_border_width))
    {
        this->setFixedWidth(this->parentWidget()->width() - m_window_border_width);
    }

    QWidget::paintEvent(event);
}

void ConfigTitleBar::mousePressEvent(QMouseEvent *event)
{
    m_is_pressed = true;
    m_start_move_pos = event->globalPos();

    return QWidget::mousePressEvent(event);
}

void ConfigTitleBar::mouseMoveEvent(QMouseEvent *event)
{
    if (m_is_pressed)
    {
        QPoint movePoint = event->globalPos() - m_start_move_pos;
        QPoint widgetPos = m_parent->pos();
        m_start_move_pos = event->globalPos();
        m_parent->move(widgetPos.x() + movePoint.x(), widgetPos.y() + movePoint.y());
    }
    return QWidget::mouseMoveEvent(event);
}

void ConfigTitleBar::mouseReleaseEvent(QMouseEvent *event)
{
    m_is_pressed = false;
    return QWidget::mouseReleaseEvent(event);
}

}  // namespace vtk::display
