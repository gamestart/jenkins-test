/****************************************************************************
 *  @file     ocralgo.cpp
 *  @brief    ocr识别算法
 *  @author   junjie.zeng
 *  @email    junjie.zeng@smartmore.com
 *  @version
 *  @date     2020.11.04
 *  Copyright (c) 2020 SmartMore All rights reserved.
 ****************************************************************************/

#include "ocralgo.h"
#include "VimoOCRModule.h"
#include "aialgomanager.h"
#include <QDebug>
#include <QFile>
#include <QTextCodec>
#include <QTime>

namespace vtk::algoservice
{

using Message = vtk::common::Message;

OcrAlgo::OcrAlgo(const QString &model_path, bool use_gpu)
{
    initModel(model_path, use_gpu);
}

OcrAlgo::~OcrAlgo()
{
    if (isRunning())
    {
        wait();
        quit();
    }
}

void OcrAlgo::runAIAlgo()
{
    if (m_is_initialization && m_model)
    {
        QMutexLocker lock(&m_mutex);
        QTime time;
        time.start();
        smartmore::OCRRequest req;
        smartmore::OCRResponse rsp;
        rsp.code = smartmore::QCCode::NotAvailable;
        req.image = m_input.input_image;
        req.threshold = 0.7f;
        auto result_code = m_model->Run(req, rsp);
        auto elapsed_time = time.elapsed();
        if (result_code != smartmore::ResultCode::Success)
            LOGW(OcrAlgo, "Run error: %d!", static_cast<size_t>(result_code));
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
        LOGW(OcrAlgo, "OCR model not initialization!");
    }
}

void OcrAlgo::initModel(const QString &model_path, bool use_gpu)
{
    if (model_path.isEmpty())
    {
        Message::postWarningMessage(tr("Empty ocr model path!"));
        return;
    }
    // check whether the file exists
    QFile filename(model_path);
    if (!filename.exists())
    {
        Message::postWarningMessage(tr("Invalid ocr model path!"));
        return;
    }
    try
    {
        m_model = std::make_unique<smartmore::VimoOCRModule>();
#ifdef Q_OS_WIN
        QTextCodec *gbk_code = QTextCodec::codecForName("GBK");
        auto path = std::string(gbk_code->fromUnicode(model_path).data());
#else
        auto path = model_path.toStdString();
#endif
        auto result_code = m_model->Init(path, use_gpu, 0);
        if (result_code != smartmore::ResultCode::Success)
            LOGW(OcrAlgo, "Init error: %d!", static_cast<size_t>(result_code));
        else
            m_is_initialization = true;
    }
    catch (...)
    {
        LOGW(OcrAlgo, "OCR model initialization error!");
    }
}

}  // namespace vtk::algoservice
