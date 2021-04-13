/****************************************************************************
 *  @file     fourviewswidget.h
 *  @brief    四图像视窗框架类
 *  @author   junjie.zeng
 *  @email    junjie.zeng@smartmore.com
 *  @version
 *  @date     2020.12.21
 *  Copyright (c) 2020 SmartMore All rights reserved.
 ****************************************************************************/
#ifndef FOURVIEWSWIDGET_H
#define FOURVIEWSWIDGET_H

#include "imageview.h"
#include <QWidget>

namespace Ui
{
class FourViewsWidget;
}

namespace vtk::display
{

class FourViewsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FourViewsWidget(const std::vector<ImageView::ViewInfo> &info_list, QWidget *parent = nullptr);
    ~FourViewsWidget();

private:
    Ui::FourViewsWidget *ui;
};
}  // namespace vtk::display
#endif  // FOURVIEWSWIDGET_H
