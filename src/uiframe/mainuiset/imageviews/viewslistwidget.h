/****************************************************************************
 *  @file     viewslistwidget.h
 *  @brief    缩略图像列表窗口部件类
 *  @author   junjie.zeng
 *  @email    junjie.zeng@smartmore.com
 *  @version
 *  @date     2020.12.21
 *  Copyright (c) 2020 SmartMore All rights reserved.
 ****************************************************************************/
#ifndef VIEWSLISTWIDGET_H
#define VIEWSLISTWIDGET_H

#include "viewitemwidget.h"
#include <QHash>
#include <QWidget>

namespace Ui
{
class ViewsListWidget;
}

namespace vtk::display
{

class ViewsListWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ViewsListWidget(QWidget *parent = nullptr);
    ~ViewsListWidget();

    void resetViewList(const std::vector<ImageView::ViewInfo> &view_info_list);

signals:
    void signalViewChange(const ImageView::ViewInfo &info, const cv::Mat &src_image, const cv::Mat &ai_image);

public slots:
    void slotUpdateThumbnail(const cv::Mat &mat);
    void slotUpdateThumbnail(const QUuid &, const cv::Mat &);

private:
    void init();
    void initWidgets();
    void initConnects();
    void updateThumbnail(const QUuid &view_uuid, const cv::Mat &mat);

private:
    Ui::ViewsListWidget *ui;
    std::vector<ImageView::ViewInfo> m_view_info_list;
    QHash<QUuid, cv::Mat> m_view_uuid_to_src_image_map;
    QHash<QUuid, std::vector<cv::Mat>> m_view_uuid_to_ai_images_map;
};
}  // namespace vtk::display
#endif  // VIEWSLISTWIDGET_H
