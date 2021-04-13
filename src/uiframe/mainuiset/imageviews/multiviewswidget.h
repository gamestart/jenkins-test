/****************************************************************************
 *  @file     multiviewswidget.h
 *  @brief    多图像视窗框架类（大于四视窗）
 *  @author   junjie.zeng
 *  @email    junjie.zeng@smartmore.com
 *  @version
 *  @date     2020.12.21
 *  Copyright (c) 2020 SmartMore All rights reserved.
 ****************************************************************************/
#ifndef MULTIVIEWSWIDGET_H
#define MULTIVIEWSWIDGET_H

#include "imageview.h"
#include <QWidget>

namespace Ui
{
class MultiViewsWidget;
}

namespace vtk::display
{

class MultiViewsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MultiViewsWidget(const std::vector<ImageView::ViewInfo> &info_list, QWidget *parent = nullptr);
    ~MultiViewsWidget();

private:
    Ui::MultiViewsWidget *ui;
};
}  // namespace vtk::display
#endif  // MULTIVIEWSWIDGET_H
