#ifndef TOASTWIDGET_H
#define TOASTWIDGET_H

#include <QLabel>
#include <QPaintEvent>
#include <QPropertyAnimation>
#include <QTimer>
#include <QWidget>

#include "ui_toastwidget.h"

namespace vtk::display
{
class ToastWidget : public QWidget
{
    Q_OBJECT
public:
    ToastWidget(QWidget *parent = 0);
    ~ToastWidget();
    void setText(const QString &text);

    void showAnimation(int timeout = 1000);  // 动画方式show出，默认2秒后消失

    static void showTip(const QString &text, QWidget *parent = nullptr);
    static void showErrorTip(const QString &text, QWidget *parent = nullptr);
    static void showSaveTip(const QString &text, const QString &icon_uri, QWidget *parent = nullptr);

protected:
    virtual void paintEvent(QPaintEvent *event);

private:
    QTimer *m_timer;
    QPropertyAnimation *m_show_animation;
    QPropertyAnimation *m_hide_animation;
    Ui::ToastWidget ui;
};
}  // namespace vtk::display
#endif  // TOASTWIDGET_H
