/****************************************************************************
 *  @file     viewstatusbar.h
 *  @brief    图像视窗状态条类
 *  @author   junjie.zeng
 *  @email    junjie.zeng@smartmore.com
 *  @version
 *  @date     2020.12.21
 *  Copyright (c) 2020 SmartMore All rights reserved.
 ****************************************************************************/
#ifndef VIEWSTATUSBAR_H
#define VIEWSTATUSBAR_H

#include "opencv2/opencv.hpp"
#include <QWidget>

namespace Ui
{
class ViewStatusBar;
}

namespace vtk::display
{

class ViewStatusBar : public QWidget
{
    Q_OBJECT

public:
    enum class ResultStatusType
    {
        notAvailable = 0,
        ok,
        ng
    };
    struct ResultStatus
    {
        size_t total_times{0};
        size_t infer_times{0};
        ResultStatusType type{ResultStatusType::notAvailable};
        QStringList result_labs;
        cv::Mat original_image;
        std::vector<cv::Mat> ai_images;
    };
    explicit ViewStatusBar(QWidget *parent = nullptr);
    ~ViewStatusBar();

    void updateResult(const ResultStatus &result);

signals:
    void signalUpdateImageDisplay(const cv::Mat &original_image, const cv::Mat &ai_image);

protected:
    void changeEvent(QEvent *e) override;

private:
    void init();
    void initWidgets();
    void initConnects();

private:
    Ui::ViewStatusBar *ui;
    ResultStatus m_result;
};
}  // namespace vtk::display
#endif  // VIEWSTATUSBAR_H
