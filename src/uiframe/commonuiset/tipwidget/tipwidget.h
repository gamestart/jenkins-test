/****************************************************************************
 *  @file     tipwidget.h
 *  @brief    弹出提示类
 *  @author   junjie.zeng
 *  @email    junjie.zeng@smartmore.com
 *  @version
 *  @date     2020.12.10
 *  Copyright (c) 2020 SmartMore All rights reserved.
 ****************************************************************************/

#ifndef TIPWIDGET_H
#define TIPWIDGET_H

#include <QWidget>

namespace Ui
{
class TipWidget;
}

class QPropertyAnimation;

namespace vtk::display
{

class TipWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TipWidget(const QString &text = "", const QString &movie_path = "", QWidget *parent = nullptr);
    ~TipWidget();

    void setText(const QString &text);

private:
    void init();
    void initWidget();
    void initConnects();

private:
    Ui::TipWidget *ui;
    QString m_text{""};
    QString m_movie_path{""};
    QPropertyAnimation *m_show_animation{nullptr};
    QPropertyAnimation *m_hide_animation{nullptr};
};

}  // namespace vtk::display
#endif  // TIPWIDGET_H
