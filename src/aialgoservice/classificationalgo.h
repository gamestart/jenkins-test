/****************************************************************************
 *  @file     classificationalgo.h
 *  @brief    分类算法
 *  @author   junjie.zeng
 *  @email    junjie.zeng@smartmore.com
 *  @version
 *  @date     2020.11.04
 *  Copyright (c) 2020 SmartMore All rights reserved.
 ****************************************************************************/

#ifndef CLASSIFICATIONALGO_H
#define CLASSIFICATIONALGO_H

#include "IClassificationModule.h"
#include "algobase.h"
#include <memory>

namespace vtk::algoservice
{

class ClassificationAlgo : public AlgoBase
{
    Q_OBJECT
public:
    struct Output : OutputBase
    {
        smartmore::ClassificationResponse rsp;
    };
    ClassificationAlgo(const QString &model_path, bool use_gpu);
    ~ClassificationAlgo();

protected:
    void initModel(const QString &model_path, bool use_gpu) override;
    void runAIAlgo() override;

private:
    std::unique_ptr<smartmore::IClassificationModule> m_model{nullptr};
    Output m_output;
};

}  // namespace vtk::algoservice
Q_DECLARE_METATYPE(smartmore::ClassificationResponse);
Q_DECLARE_METATYPE(vtk::algoservice::ClassificationAlgo::Output);

#endif  // CLASSIFICATIONALGO_H
