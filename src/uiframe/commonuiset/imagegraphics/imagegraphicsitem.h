/****************************************************************************
 *  @file     imagegraphicsitem.h
 *  @brief    图像图元类
 *  @author   junjie.zeng
 *  @email    junjie.zeng@smartmore.com
 *  @version
 *  @date     2020.12.23
 *  Copyright (c) 2020 SmartMore All rights reserved.
 ****************************************************************************/
#ifndef IMAGEGRAPHICSITEM_H
#define IMAGEGRAPHICSITEM_H
#include <QGraphicsPixmapItem>

namespace vtk::display
{

class ImageGraphicsItem : public QGraphicsPixmapItem
{
public:
    explicit ImageGraphicsItem(QGraphicsItem *parent = nullptr);
    ~ImageGraphicsItem() = default;
};
}  // namespace vtk::display
#endif  // IMAGEGRAPHICSITEM_H
