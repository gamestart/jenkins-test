/****************************************************************************
 *  @file     imagegraphicsview.h
 *  @brief    图像显示视图类
 *  @author   junjie.zeng
 *  @email    junjie.zeng@smartmore.com
 *  @version
 *  @date     2020.12.23
 *  Copyright (c) 2020 SmartMore All rights reserved.
 ****************************************************************************/
#ifndef IMAGEGRAPHICSVIEW_H
#define IMAGEGRAPHICSVIEW_H
#include <QGraphicsView>
#include <QImage>

namespace vtk::display
{
class ImageGraphicsScene;
class ImageGraphicsItem;

class ImageGraphicsView : public QGraphicsView
{
    Q_OBJECT
public:
    ImageGraphicsView(QWidget *parent = nullptr);
    ~ImageGraphicsView() = default;

    void setImage(const QImage &image, bool revert);
    void zoomoutImage();
    void zoominImage();
    void revertImage();

signals:
    void signalUpdateFactor(float factor);
    void signalClicked();

protected:
    void wheelEvent(QWheelEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    void scaleImage(float factor);

private:
    ImageGraphicsScene *m_scene{nullptr};
    ImageGraphicsItem *m_item{nullptr};
    float m_scale_factor{1.0};
    int m_view_factor{100};
    const std::vector<int> m_view_factor_list{100, 110, 125, 150, 175, 200, 300, 400, 500};
    bool m_is_mouse_pressed{false};  //按下鼠标左键
};
}  // namespace vtk::display
#endif  // IMAGEGRAPHICSVIEW_H
