/****************************************************************************
 *  @file     segmentationalgo.h
 *  @brief    分割类算法
 *  @author   junjie.zeng
 *  @email    junjie.zeng@smartmore.com
 *  @version
 *  @date     2020.11.04
 *  Copyright (c) 2020 SmartMore All rights reserved.
 ****************************************************************************/

#ifndef SEGMENTATIONALGO_H
#define SEGMENTATIONALGO_H

#include "ISegmentationModule.h"
#include "algobase.h"
#include <memory>

namespace vtk::algoservice
{

class SegmentationAlgo : public AlgoBase
{
    Q_OBJECT
public:
    struct Output : OutputBase
    {
        smartmore::SegmentationResponse rsp;
    };
    SegmentationAlgo(const QString &model_path, bool use_gpu);
    ~SegmentationAlgo();

protected:
    void initModel(const QString &model_path, bool use_gpu) override;
    void runAIAlgo() override;

private:
    std::unique_ptr<smartmore::ISegmentationModule> m_model{nullptr};
    Output m_output;
};

}  // namespace vtk::algoservice
Q_DECLARE_METATYPE(smartmore::SegmentationResponse);
Q_DECLARE_METATYPE(vtk::algoservice::SegmentationAlgo::Output);

#endif  // SEGMENTATIONALGO_H
