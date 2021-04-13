/****************************************************************************
 *  @file     arrowwidget.cpp
 *  @brief    分类算法
 *  @author   leyan.zhu
 *  @email    leyan.zhu@smartmore.com
 *  @version
 *  @date     2020.12.10
 *  Copyright (c) 2020 SmartMore All rights reserved.
 ****************************************************************************/

#include "arrowwidget.h"

#include <QGraphicsDropShadowEffect>
#include <QHBoxLayout>
#include <QPainter>

#define SHADOW_WIDTH 1      // 窗口阴影宽度;
#define TRIANGLE_WIDTH 15   // 小三角的宽度;
#define TRIANGLE_HEIGHT 10  // 小三角的高度;
#define BORDER_RADIUS 4     // 窗口边角的弧度;
namespace vtk::display
{

ArrowWidget::ArrowWidget(QWidget *parent)
    : QWidget(parent), m_start_x(50), m_triangle_width(TRIANGLE_WIDTH), m_triangle_height(TRIANGLE_HEIGHT)
{
    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    // setStyleSheet("background: rgba(0,0,0,0.7);");

    // 设置阴影边框;
    auto shadow_effect = new QGraphicsDropShadowEffect(this);
    shadow_effect->setOffset(0, 0);
    shadow_effect->setColor(Qt::gray);
    shadow_effect->setBlurRadius(SHADOW_WIDTH);
    this->setGraphicsEffect(shadow_effect);

    setFixedSize(200, 200);
}

void ArrowWidget::setCenterWidget(QWidget *widget)
{
    QHBoxLayout *main_layout = new QHBoxLayout(this);
    main_layout->addWidget(widget);
    main_layout->setSpacing(0);
    main_layout->setContentsMargins(10, 10 + TRIANGLE_HEIGHT, 10, 10);
}

// 设置小三角显示的起始位置;
void ArrowWidget::setStartPos(int start_x)
{
    m_start_x = start_x;
}

void ArrowWidget::setTriangleInfo(int width, int height)
{
    m_triangle_width = width;
    m_triangle_height = height;
}

void ArrowWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(100, 100, 100));

    // 小三角区域;
    QPolygon triangle_polygon;
    triangle_polygon << QPoint(m_start_x, m_triangle_height + SHADOW_WIDTH);
    triangle_polygon << QPoint(m_start_x + m_triangle_width / 2, SHADOW_WIDTH);
    triangle_polygon << QPoint(m_start_x + m_triangle_width, m_triangle_height + SHADOW_WIDTH);

    QPainterPath draw_path;
    draw_path.addRoundedRect(QRect(SHADOW_WIDTH, m_triangle_height + SHADOW_WIDTH, width() - SHADOW_WIDTH * 2,
                                   height() - SHADOW_WIDTH * 2 - m_triangle_height),
                             BORDER_RADIUS, BORDER_RADIUS);
    // Rect + Triangle;
    draw_path.addPolygon(triangle_polygon);
    painter.drawPath(draw_path);
}
}  // namespace vtk::display
