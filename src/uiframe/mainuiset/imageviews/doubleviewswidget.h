/****************************************************************************
 *  @file     doubleviewswidget.h
 *  @brief    两图像视窗框架类
 *  @author   junjie.zeng
 *  @email    junjie.zeng@smartmore.com
 *  @version
 *  @date     2020.12.21
 *  Copyright (c) 2020 SmartMore All rights reserved.
 ****************************************************************************/
#ifndef DOUBLEVIEWSWIDGET_H
#define DOUBLEVIEWSWIDGET_H

#include "imageview.h"
#include <QWidget>

namespace Ui
{
class DoubleViewsWidget;
}

namespace vtk::display
{

class DoubleViewsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DoubleViewsWidget(const std::vector<ImageView::ViewInfo> &info_list, QWidget *parent = nullptr);
    ~DoubleViewsWidget();

private:
    Ui::DoubleViewsWidget *ui;
};
}  // namespace vtk::display
#endif  // DOUBLEVIEWSWIDGET_H
