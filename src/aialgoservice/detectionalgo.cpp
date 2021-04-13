/****************************************************************************
 *  @file     detectionalgo.cpp
 *  @brief    检测类算法
 *  @author   junjie.zeng
 *  @email    junjie.zeng@smartmore.com
 *  @version
 *  @date     2020.11.04
 *  Copyright (c) 2020 SmartMore All rights reserved.
 ****************************************************************************/

#include "detectionalgo.h"
#include "VimoDetectionModule.h"
#include <QDebug>
#include <QFile>
#include <QTextCodec>
#include <QTime>

namespace vtk::algoservice
{

using Message = vtk::common::Message;

DetectionAlgo::DetectionAlgo(const QString &model_path, bool use_gpu)
{
    initModel(model_path, use_gpu);
}

DetectionAlgo::~DetectionAlgo()
{
    if (isRunning())
    {
        wait();
        quit();
    }
}

void DetectionAlgo::runAIAlgo()
{
    if (m_is_initialization && m_model)
    {
        QMutexLocker lock(&m_mutex);
        QTime time;
        time.start();
        smartmore::DetectionRequest req = {m_input.input_image, 0.4f};
        smartmore::DetectionResponse rsp;
        rsp.code = smartmore::QCCode::NotAvailable;
        auto result_code = m_model->Run(req, rsp);
        auto elapsed_time = time.elapsed();
        if (result_code != smartmore::ResultCode::Success)
            LOGW(DetectionAlgo, "Run error: %d!", static_cast<size_t>(result_code));
        m_model->Visualize(req, rsp, m_output.output_image);
        m_output.trigger_char = m_input.trigger_char;
        m_output.algo_handle = m_input.algo_handle;
        m_output.image_uuid = m_input.image_uuid;
        m_output.input_image = std::move(m_input.input_image);
        m_output.elapsed_time = elapsed_time;
        m_output.rsp = std::move(rsp);
        emit signalUpdateResult(&m_output);
    }
    else
    {
        LOGW(DetectionAlgo, "Detection model not initialization!");
    }
}

void DetectionAlgo::initModel(const QString &model_path, bool use_gpu)
{
    if (model_path.isEmpty())
    {
        Message::postWarningMessage(tr("Empty detection model path!"));
        return;
    }
    // check whether the file exists
    QFile filename(model_path);
    if (!filename.exists())
    {
        Message::postWarningMessage(tr("Invalid detection model path!"));
        return;
    }
    try
    {
        m_model = std::make_unique<smartmore::VimoDetectionModule>();
#ifdef Q_OS_WIN
        QTextCodec *gbk_code = QTextCodec::codecForName("GBK");
        auto path = std::string(gbk_code->fromUnicode(model_path).data());
#else
        auto path = model_path.toStdString();
#endif
        auto result_code = m_model->Init(path, use_gpu, 0);
        if (result_code != smartmore::ResultCode::Success)
            LOGW(DetectionAlgo, "Init error: %d!", static_cast<size_t>(result_code));
        else
            m_is_initialization = true;
    }
    catch (...)
    {
        LOGW(DetectionAlgo, "Detection model initialization error!");
    }
}

}  // namespace vtk::algoservice
