/****************************************************************************
 *  @file     aialgomanager.h
 *  @brief    ai算法管理员
 *  @author   junjie.zeng
 *  @email    junjie.zeng@smartmore.com
 *  @version
 *  @date     2020.10.21
 *  Copyright (c) 2020 SmartMore All rights reserved.
 ****************************************************************************/

#ifndef AIALGOMANAGER_H
#define AIALGOMANAGER_H

#include "algobase.h"
#include <QMutex>
#include <QObject>
#include <QVariant>
#include <memory>
#include <unordered_map>

namespace vtk::algoservice
{

class AIAlgoManager : public QObject
{
    Q_OBJECT
public:
    using AlgoType = vtk::common::AlgoType;
    using AlgoHandle = vtk::common::AlgoHandle;
    using ResultStatusType = vtk::common::ResultStatusType;

    struct AlgoResult : vtk::common::AlgoResult
    {
        QString trigger_char;
        common::AlgoHandle algo_handle;
        QUuid image_uuid;
    };

    static AIAlgoManager &getInstance();
    ~AIAlgoManager() = default;

    void clearAlgos();
    AlgoHandle initAlgo(AlgoType type, const QString &model_path = "", bool use_gpu = false);
    void runAlgo(AlgoHandle handle, const QString &trigger_char, const cv::Mat &image,
                 const QUuid &image_uuid = QUuid());

    inline const std::unordered_map<AlgoHandle, cv::Mat> &getAIImages() const
    {
        return m_ai_images_buff_map;
    }

signals:
    void signalUpdateResult(const AlgoResult &result);

private:
    AIAlgoManager() = default;
    DISALLOW_COPY_AND_ASSIGN(AIAlgoManager)

private:
    std::unordered_map<AlgoHandle, std::unique_ptr<AlgoBase>> m_algo_map;
    std::unordered_map<AlgoHandle, cv::Mat> m_ai_images_buff_map;  //<handle, ai_images_buff>
};

}  // namespace vtk::algoservice
#endif  // AIALGOMANAGER_H
