/****************************************************************************
 *  @file     algobase.h
 *  @brief    算法接口基类
 *  @author   junjie.zeng
 *  @email    junjie.zeng@smartmore.com
 *  @version
 *  @date     2020.11.04
 *  Copyright (c) 2020 SmartMore All rights reserved.
 ****************************************************************************/

#ifndef ALGOBASE_H
#define ALGOBASE_H

#include "common/vtkcommon.h"
#include <QMutex>
#include <QThread>
#include <QUuid>
#include <opencv2/opencv.hpp>

namespace vtk::algoservice
{

class AlgoBase : public QThread
{
    Q_OBJECT
public:
    struct Input
    {
        QString trigger_char;
        common::AlgoHandle algo_handle;
        cv::Mat input_image;
        QUuid image_uuid;
    };

    struct OutputBase
    {
        virtual ~OutputBase() = default;
        size_t elapsed_time{0};
        QString trigger_char;
        common::AlgoHandle algo_handle;
        cv::Mat input_image;
        cv::Mat output_image;
        QUuid image_uuid;
    };

    AlgoBase() = default;
    virtual ~AlgoBase() = default;

    void setInput(const Input &image);
    const Input &getInput();

    bool isInitialized()
    {
        return m_is_initialization;
    }

signals:
    void signalUpdateResult(OutputBase *output);

protected:
    virtual void initModel(const QString &model_path, bool use_gpu) = 0;
    virtual void runAIAlgo() = 0;
    void run() override;

protected:
    bool m_is_initialization{false};
    Input m_input;
    QMutex m_mutex;
};

}  // namespace vtk::algoservice
#endif  // ALGOBASE_H
