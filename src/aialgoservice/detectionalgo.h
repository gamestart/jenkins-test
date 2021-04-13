/****************************************************************************
 *  @file     detectionalgo.h
 *  @brief    检测类算法
 *  @author   junjie.zeng
 *  @email    junjie.zeng@smartmore.com
 *  @version
 *  @date     2020.11.04
 *  Copyright (c) 2020 SmartMore All rights reserved.
 ****************************************************************************/

#ifndef DETECTIONALGO_H
#define DETECTIONALGO_H

#include "IDetectionModule.h"
#include "algobase.h"
#include <memory>

namespace vtk::algoservice
{

class DetectionAlgo : public AlgoBase
{
    Q_OBJECT
public:
    struct Output : OutputBase
    {
        smartmore::DetectionResponse rsp;
    };
    DetectionAlgo(const QString &model_path, bool use_gpu);
    ~DetectionAlgo();

protected:
    void initModel(const QString &model_path, bool use_gpu) override;
    void runAIAlgo() override;

private:
    std::unique_ptr<smartmore::IDetectionModule> m_model{nullptr};
    Output m_output;
};

}  // namespace vtk::algoservice
Q_DECLARE_METATYPE(smartmore::DetectionResponse);
Q_DECLARE_METATYPE(vtk::algoservice::DetectionAlgo::Output);

#endif  // DETECTIONALGO_H
