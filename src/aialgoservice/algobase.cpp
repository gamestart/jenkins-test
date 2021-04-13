/****************************************************************************
 *  @file     algobase.cpp
 *  @brief    相机设备基类
 *  @author   junjie.zeng
 *  @email    junjie.zeng@smartmore.com
 *  @version
 *  @date     2020.11.04
 *  Copyright (c) 2020 SmartMore All rights reserved.
 ****************************************************************************/

#include "algobase.h"
#include <QMutexLocker>

namespace vtk::algoservice
{

void AlgoBase::setInput(const Input &input)
{
    QMutexLocker lock(&m_mutex);
    m_input = input;
}

const AlgoBase::Input &AlgoBase::getInput()
{
    QMutexLocker lock(&m_mutex);
    return m_input;
}

void AlgoBase::run()
{
    runAIAlgo();
}

}  // namespace vtk::algoservice
