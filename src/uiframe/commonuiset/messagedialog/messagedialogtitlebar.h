#ifndef MESSAGEDIALOGTITLEBAR_H
#define MESSAGEDIALOGTITLEBAR_H

#include <QLabel>
#include <QWidget>

namespace vtk::display
{

class MessageDialogTitlebar : public QWidget
{
    Q_OBJECT
public:
    explicit MessageDialogTitlebar(QWidget *parent = 0);
    QLabel *m_title{nullptr};  // 标题栏内容;

private:
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

private:
    QWidget *m_parent{nullptr};
    // 标题栏背景色;
    int m_color_r;
    int m_color_g;
    int m_color_b;

    // 移动窗口的变量;
    bool m_is_pressed;
    QPoint m_start_move_pos;

    // 窗口边框宽度;
    int m_window_border_width;

    void initControl();
    void setTitleContent(QString);
};
}  // namespace vtk::display
#endif  // MESSAGEDIALOGTITLEBAR_H
