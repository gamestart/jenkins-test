/****************************************************************************
 *  @file     imagegraphicsview.cpp
 *  @brief    图像显示视图类
 *  @author   junjie.zeng
 *  @email    junjie.zeng@smartmore.com
 *  @version
 *  @date     2020.12.23
 *  Copyright (c) 2020 SmartMore All rights reserved.
 ****************************************************************************/
#include "imagegraphicsview.h"
#include "imagegraphicsitem.h"
#include "imagegraphicsscene.h"
#include <QScrollBar>
#include <QWheelEvent>

namespace vtk::display
{

ImageGraphicsView::ImageGraphicsView(QWidget *parent) : QGraphicsView(parent)
{
    setMouseTracking(true);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setResizeAnchor(QGraphicsView::AnchorUnderMouse);
    m_scene = new ImageGraphicsScene(this);
    m_item = new ImageGraphicsItem;
    m_scene->addItem(m_item);
    setScene(m_scene);
    m_item->setFlag(QGraphicsItem::ItemIsMovable, true);
    m_item->setTransformationMode(Qt::SmoothTransformation);
    setAlignment(Qt::AlignCenter);
}

void ImageGraphicsView::setImage(const QImage &image, bool revert)
{
    auto size_changed = m_item->pixmap().size() != image.size();
    m_item->setPixmap(QPixmap::fromImage(image));
    if (revert || size_changed)
        revertImage();
}

void ImageGraphicsView::zoomoutImage()
{
    if (!m_item->pixmap().isNull())
    {
        int new_view_scale_factor = m_view_factor;
        auto scale_itr = std::find(m_view_factor_list.begin(), m_view_factor_list.end(), m_view_factor);
        if (scale_itr != m_view_factor_list.end())
        {
            int scale_index = distance(m_view_factor_list.begin(), scale_itr);

            if (scale_index)
            {
                new_view_scale_factor = m_view_factor_list[scale_index - 1];
            }
        }
        scaleImage((float)new_view_scale_factor / m_view_factor);
        m_view_factor = new_view_scale_factor;
    }
}

void ImageGraphicsView::zoominImage()
{
    if (!m_item->pixmap().isNull())
    {
        int new_shown_scale_factor = m_view_factor;
        auto scale_itr = std::find(m_view_factor_list.begin(), m_view_factor_list.end(), m_view_factor);
        if (scale_itr != m_view_factor_list.end())
        {
            int scale_index = distance(m_view_factor_list.begin(), scale_itr);

            if (scale_index != m_view_factor_list.size() - 1)
            {
                new_shown_scale_factor = m_view_factor_list[scale_index + 1];
            }
        }
        scaleImage((float)new_shown_scale_factor / m_view_factor);
        m_view_factor = new_shown_scale_factor;
    }
}

void ImageGraphicsView::scaleImage(float factor)
{
    m_scale_factor *= factor;
    QMatrix matrix;
    matrix.scale(m_scale_factor, m_scale_factor);
    setMatrix(matrix);
    m_view_factor = factor * m_view_factor;
    emit signalUpdateFactor(m_scale_factor);
}

void ImageGraphicsView::revertImage()
{
    if (!m_item->pixmap().isNull())
    {
        m_scene->setSceneRect(0, 0, m_item->pixmap().width(), m_item->pixmap().height());
        fitInView(m_item, Qt::KeepAspectRatio);
        m_scale_factor = matrix().m11();
        m_view_factor = 100;
        emit signalUpdateFactor(m_scale_factor);
        QPoint new_pos;
        auto rect = viewport()->geometry();
        auto item_rect = mapFromScene(m_item->scenePos().x(), m_item->scenePos().y(), m_item->boundingRect().width(),
                                      m_item->boundingRect().height())
                             .boundingRect();
        new_pos.setX((rect.width() - item_rect.width()) / 2);
        new_pos.setY((rect.height() - item_rect.height()) / 2);
        m_item->setPos(mapToScene(new_pos));
    }
}

void ImageGraphicsView::wheelEvent(QWheelEvent *event)
{
    auto cursor_point = event->pos();
    auto scene_pos = mapToScene(QPoint(cursor_point.x(), cursor_point.y()));
    auto view_width = static_cast<qreal>(viewport()->width());
    auto view_height = static_cast<qreal>(viewport()->height());
    auto h_scale = cursor_point.x() / view_width;
    auto v_scale = cursor_point.y() / view_height;
    event->delta() > 0 ? zoominImage() : zoomoutImage();
    emit signalUpdateFactor(m_scale_factor);

    auto view_point = matrix().map(scene_pos);
    horizontalScrollBar()->setValue(static_cast<int>(view_point.x() - view_width * h_scale));
    verticalScrollBar()->setValue(static_cast<int>(view_point.y() - view_height * v_scale));
}

void ImageGraphicsView::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        m_is_mouse_pressed = true;
    }
    QGraphicsView::mousePressEvent(event);
}

void ImageGraphicsView::mouseMoveEvent(QMouseEvent *event)
{
    if (m_is_mouse_pressed)
    {
        this->setCursor(Qt::OpenHandCursor);
    }
    QGraphicsView::mouseMoveEvent(event);
}

void ImageGraphicsView::mouseReleaseEvent(QMouseEvent *event)
{
    auto new_pos = mapFromScene(m_item->scenePos());
    auto rect = viewport()->geometry();
    auto item_rect = mapFromScene(m_item->scenePos().x(), m_item->scenePos().y(), m_item->boundingRect().width(),
                                  m_item->boundingRect().height())
                         .boundingRect();

    auto view_center_pos = QPoint(rect.width() / 2, rect.height() / 2);
    auto item_center_pos = item_rect.center();
    auto width = (rect.width() + item_rect.width()) / 2;
    auto height = (rect.height() + item_rect.height()) / 2;

    auto d_width = abs(item_center_pos.x() - view_center_pos.x());
    auto d_height = abs(item_center_pos.y() - view_center_pos.y());
    if (d_width >= width && d_height >= height)
    {
        new_pos.setX(new_pos.x() >= rect.right() ? (rect.right() - 10) : (rect.left() - item_rect.width() + 10));
        new_pos.setY(rect.bottom() <= new_pos.y() ? (rect.bottom() - 10) : (rect.top() - item_rect.height() + 10));
    }
    else if (d_width >= width)
        new_pos.setX(new_pos.x() >= rect.right() ? (rect.right() - 10) : (rect.left() - item_rect.width() + 10));
    else if (d_height >= height)
        new_pos.setY(rect.bottom() <= new_pos.y() ? (rect.bottom() - 10) : (rect.top() - item_rect.height() + 10));

    m_item->setPos(mapToScene(new_pos));

    if (event->button() == Qt::LeftButton)
    {
        m_is_mouse_pressed = false;
        this->setCursor(Qt::ArrowCursor);
    }
    emit signalClicked();
    QGraphicsView::mouseReleaseEvent(event);
}

}  // namespace vtk::display
