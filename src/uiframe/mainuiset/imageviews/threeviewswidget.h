/****************************************************************************
 *  @file     threeviewswidget.h
 *  @brief    三图像视窗框架类
 *  @author   junjie.zeng
 *  @email    junjie.zeng@smartmore.com
 *  @version
 *  @date     2020.12.21
 *  Copyright (c) 2020 SmartMore All rights reserved.
 ****************************************************************************/
#ifndef THREEVIEWSWIDGET_H
#define THREEVIEWSWIDGET_H

#include "imageview.h"
#include <QWidget>

namespace Ui
{
class ThreeViewsWidget;
}

namespace vtk::display
{

class ThreeViewsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ThreeViewsWidget(const std::vector<ImageView::ViewInfo> &info_list, QWidget *parent = nullptr);
    ~ThreeViewsWidget();

private:
    Ui::ThreeViewsWidget *ui;
};
}  // namespace vtk::display
#endif  // THREEVIEWSWIDGET_H
