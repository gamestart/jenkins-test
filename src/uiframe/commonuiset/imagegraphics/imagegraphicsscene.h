/****************************************************************************
 *  @file     imagegraphicsscene.h
 *  @brief    图像显示场景类
 *  @author   junjie.zeng
 *  @email    junjie.zeng@smartmore.com
 *  @version
 *  @date     2020.12.23
 *  Copyright (c) 2020 SmartMore All rights reserved.
 ****************************************************************************/
#ifndef IMAGEGRAPHICSSCENE_H
#define IMAGEGRAPHICSSCENE_H
#include <QGraphicsScene>

namespace vtk::display
{

class ImageGraphicsScene : public QGraphicsScene
{
public:
    ImageGraphicsScene(QObject *parent = nullptr);
    ~ImageGraphicsScene() = default;
};
}  // namespace vtk::display
#endif  // IMAGEGRAPHICSSCENE_H
