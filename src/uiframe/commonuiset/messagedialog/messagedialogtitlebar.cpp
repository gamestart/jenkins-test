#include "messagedialogtitlebar.h"

#include <QApplication>
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QPainter>
#include <QTranslator>

namespace vtk::display
{

MessageDialogTitlebar::MessageDialogTitlebar(QWidget *parent)
    : QWidget(parent), m_color_r(240), m_color_g(240), m_color_b(240), m_is_pressed(false), m_window_border_width(0),
      m_parent(parent)
{
    initControl();
}

void MessageDialogTitlebar::initControl()
{
    m_title = new QLabel;
    m_title->setStyleSheet("color: rgba(0,0,0,0.8); background: transparent; font-size: 18px;");

    QHBoxLayout *mylayout = new QHBoxLayout(this);
    mylayout->addWidget(m_title);
    mylayout->setContentsMargins(20, 0, 0, 0);

    this->setFixedWidth(500);
    this->setFixedHeight(50);
    this->setWindowFlags(Qt::FramelessWindowHint);
}

/*************事件处理函数**********************/
void MessageDialogTitlebar::paintEvent(QPaintEvent *event)
{

    //设置背景色
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

//鼠标按下事件
//通过mousePressEvent、mouseMoveEvent、mouseReleaseEvent三个事件实现了鼠标拖动标题栏移动窗口的效果;
void MessageDialogTitlebar::mousePressEvent(QMouseEvent *event)
{
    m_is_pressed = true;
    m_start_move_pos = event->globalPos();

    return QWidget::mousePressEvent(event);
}

//鼠标移动事件
void MessageDialogTitlebar::mouseMoveEvent(QMouseEvent *event)
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

void MessageDialogTitlebar::mouseReleaseEvent(QMouseEvent *event)
{
    m_is_pressed = false;
    return QWidget::mouseReleaseEvent(event);
}

}  // namespace vtk::display
