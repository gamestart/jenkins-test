/****************************************************************************
 *  @file     viewitemwidget.h
 *  @brief    缩略图像item部件类
 *  @author   junjie.zeng
 *  @email    junjie.zeng@smartmore.com
 *  @version
 *  @date     2020.12.24
 *  Copyright (c) 2020 SmartMore All rights reserved.
 ****************************************************************************/
#ifndef VIEWITEMWIDGET_H
#define VIEWITEMWIDGET_H

#include "imageview.h"
#include <QWidget>

namespace Ui
{
class ViewItemWidget;
}

namespace vtk::display
{

class ViewItemWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ViewItemWidget(const ImageView::ViewInfo &view_info, QWidget *parent = nullptr);
    ~ViewItemWidget();

    void setThumbnail(const QImage &image);

private:
    void init();
    void initWidgets();
    void initConnects();

private:
    Ui::ViewItemWidget *ui;
    const ImageView::ViewInfo &m_view_info;
};
}  // namespace vtk::display
#endif  // VIEWITEMWIDGET_H
