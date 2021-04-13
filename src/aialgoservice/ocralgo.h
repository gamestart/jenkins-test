/****************************************************************************
 *  @file     ocralgo.h
 *  @brief    ocr识别算法
 *  @author   junjie.zeng
 *  @email    junjie.zeng@smartmore.com
 *  @version
 *  @date     2020.11.04
 *  Copyright (c) 2020 SmartMore All rights reserved.
 ****************************************************************************/

#ifndef OCRALGO_H
#define OCRALGO_H

#include "IOCRModule.h"
#include "algobase.h"
#include <memory>

namespace vtk::algoservice
{

class OcrAlgo : public AlgoBase
{
    Q_OBJECT
public:
    struct Output : OutputBase
    {
        smartmore::OCRResponse rsp;
    };
    OcrAlgo(const QString &model_path, bool use_gpu);
    ~OcrAlgo();

protected:
    void initModel(const QString &model_path, bool use_gpu) override;
    void runAIAlgo() override;

private:
    std::unique_ptr<smartmore::IOCRModule> m_model{nullptr};
    Output m_output;
};

}  // namespace vtk::algoservice
Q_DECLARE_METATYPE(smartmore::OCRResponse);
Q_DECLARE_METATYPE(vtk::algoservice::OcrAlgo::Output);
#endif  // OCRALGO_H
