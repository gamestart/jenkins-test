/****************************************************************************
 *  @file     aialgomanager.cpp
 *  @brief    ai算法管理员
 *  @author   junjie.zeng
 *  @email    junjie.zeng@smartmore.com
 *  @version
 *  @date     2020.10.21
 *  Copyright (c) 2020 SmartMore All rights reserved.
 ****************************************************************************/

#include "aialgomanager.h"
#include "classificationalgo.h"
#include "common/vtkcommon.h"
#include "detectionalgo.h"
#include "ocralgo.h"
#include "segmentationalgo.h"
#include <QDebug>
#include <QMutexLocker>

namespace vtk::algoservice
{

AIAlgoManager &AIAlgoManager::getInstance()
{
    static AIAlgoManager s_algos_manager;
    return s_algos_manager;
}

void AIAlgoManager::clearAlgos()
{
    for (auto &algo : m_algo_map)
        algo.second.reset();
    m_algo_map.clear();
}

AIAlgoManager::AlgoHandle AIAlgoManager::initAlgo(AlgoType type, const QString &model_path, bool use_gpu)
{
    AlgoHandle new_handle = m_algo_map.size() + 1;
    switch (type)
    {
        case AlgoType::segmentationType:
        {
            m_algo_map.insert(std::make_pair(new_handle, std::make_unique<SegmentationAlgo>(model_path, use_gpu)));
            break;
        }
        case AlgoType::detectionType:
        {
            m_algo_map.insert(std::make_pair(new_handle, std::make_unique<DetectionAlgo>(model_path, use_gpu)));
            break;
        }
        case AlgoType::classificationType:
        {
            m_algo_map.insert(std::make_pair(new_handle, std::make_unique<ClassificationAlgo>(model_path, use_gpu)));
            break;
        }
        case AlgoType::ocrType:
        {
            m_algo_map.insert(std::make_pair(new_handle, std::make_unique<OcrAlgo>(model_path, use_gpu)));
            break;
        }
        default:
            m_algo_map.insert(std::make_pair(new_handle, nullptr));
            break;
    }
    auto it = m_algo_map.find(new_handle);
    if (it != m_algo_map.end() && it->second)
    {
        auto &algo_ptr = it->second;
        connect(algo_ptr.get(), &AlgoBase::signalUpdateResult, this, [=](AlgoBase::OutputBase *output) {
            if (!output)
                return;

            LOGT(AIAlgoManager, "received algo result!");
            if (m_ai_images_buff_map.find(output->algo_handle) == m_ai_images_buff_map.end())
                m_ai_images_buff_map.emplace(std::make_pair(output->algo_handle, output->output_image));
            else
                m_ai_images_buff_map.at(output->algo_handle) = output->output_image;

            AlgoResult algo_result;
            algo_result.elapsed_time = output->elapsed_time;
            algo_result.trigger_char = output->trigger_char;
            algo_result.algo_handle = output->algo_handle;
            algo_result.image = output->input_image;
            algo_result.ai_image = output->output_image;
            algo_result.image_uuid = output->image_uuid;

            std::string labs;
            switch (type)
            {
                case AlgoType::segmentationType:
                {
                    auto result = dynamic_cast<SegmentationAlgo::Output *>(output);
                    if (result)
                    {
                        //  对结果再做一层处理，然后转发到界面，显示需要的数据
                        // algo_result.result_status = static_cast<ResultStatusType>(result->rsp.code);
                        if (!result->rsp.mask.data)
                            algo_result.result_status = ResultStatusType::ok;
                        else
                            algo_result.result_status = ResultStatusType::ng;
                        for (auto name : result->rsp.names)
                        {
                            algo_result.lab_names.emplace_back(name.second);
                            labs += name.second + " ";
                        }
                        emit signalUpdateResult(algo_result);
                    }
                    break;
                }
                case AlgoType::detectionType:
                {
                    auto result = dynamic_cast<DetectionAlgo::Output *>(output);
                    if (result)
                    {
                        // 对结果再做一层处理，然后转发到界面，显示需要的数据
                        // algo_result.result_status = static_cast<ResultStatusType>(result->rsp.code);
                        if (!result->rsp.box_list.size())
                            algo_result.result_status = ResultStatusType::ok;
                        else
                            algo_result.result_status = ResultStatusType::ng;
                        for (auto box : result->rsp.box_list)
                        {
                            algo_result.lab_names.emplace_back(box.label_name);
                            labs += box.label_name + " ";
                        }
                        emit signalUpdateResult(algo_result);
                    }
                    break;
                }
                case AlgoType::classificationType:
                {
                    auto result = dynamic_cast<ClassificationAlgo::Output *>(output);
                    if (result)
                    {
                        //  对结果再做一层处理，然后转发到界面，显示需要的数据
                        // algo_result.result_status = static_cast<ResultStatusType>(result->rsp.code);
                        algo_result.result_status = ResultStatusType::notAvailable;
                        for (auto lab : result->rsp.labels)
                        {
                            algo_result.lab_names.emplace_back(lab.name + " " + std::to_string(lab.score));
                            labs += lab.name + " ";
                            labs += std::to_string(lab.score) + " ";
                        }
                        emit signalUpdateResult(algo_result);
                    }
                    break;
                }
                case AlgoType::ocrType:
                {
                    auto result = dynamic_cast<OcrAlgo::Output *>(output);
                    if (result)
                    {
                        //  对结果再做一层处理，然后转发到界面，显示需要的数据
                        // algo_result.result_status = static_cast<ResultStatusType>(result->rsp.code);
                        algo_result.result_status = ResultStatusType::notAvailable;
                        for (auto block : result->rsp.blocks)
                        {
                            algo_result.lab_names.emplace_back(block.text);
                            labs += block.text + " ";
                        }
                        emit signalUpdateResult(algo_result);
                    }
                    break;
                }
                default:
                    break;
            }
            LOGI(AIAlgoManager, "algo type: %d", type);
            LOGI(AIAlgoManager, "algo elapsed time: %d", algo_result.elapsed_time);
            LOGI(AIAlgoManager, "algo algo handle: %d", algo_result.algo_handle);
            LOGI(AIAlgoManager, "algo trigger char: %s", algo_result.trigger_char.toLatin1().data());
            LOGI(AIAlgoManager, "algo lab names: %s", labs.c_str());
        });
    }
    return new_handle;
}

void AIAlgoManager::runAlgo(AlgoHandle handle, const QString &trigger_char, const cv::Mat &image,
                            const QUuid &image_uuid)
{
    auto it = m_algo_map.find(handle);
    if (it != m_algo_map.end() && it->second)
    {
        auto &algo_ptr = it->second;
        if (algo_ptr->isInitialized() && !image.empty())
        {
            AlgoBase::Input input{trigger_char, handle, image, image_uuid};
            algo_ptr->setInput(input);
            algo_ptr->start();
            LOGI(AIAlgoManager, "algo is running!");
        }
        else
        {
            //发送原图到界面显示
            AlgoResult algo_result;
            algo_result.image = image;
            algo_result.trigger_char = trigger_char;
            algo_result.algo_handle = handle;
            algo_result.image_uuid = image_uuid;
            emit signalUpdateResult(algo_result);
            LOGW(AIAlgoManager, "algo is uninitialized!");
        }
    }
    else
    {
        //发送原图到界面显示
        AlgoResult algo_result;
        algo_result.image = image;
        algo_result.trigger_char = trigger_char;
        algo_result.algo_handle = handle;
        algo_result.image_uuid = image_uuid;
        emit signalUpdateResult(algo_result);
        LOGW(AIAlgoManager, "algo type is noType!");
    }
}

}  // namespace vtk::algoservice
