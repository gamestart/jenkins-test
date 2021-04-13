/****************************************************************************
 *  @file     runtimecontroller.h
 *  @brief    vtk运行控制类
 *  @author   xiaoyu.liu
 *  @email    xiaoyu.liu@smartmore.com
 *  @version
 *  @date     2020-12-25
 *  Copyright (c) 2020 SmartMore All rights reserved.
 ****************************************************************************/
#ifndef RUNTIMECONTROLLER_H
#define RUNTIMECONTROLLER_H

#include "aialgoservice/aialgomanager.h"
#include "common/vtkcommon.h"
#include "statistics.h"
#include <QHash>
#include <QImage>
#include <QList>
#include <QObject>
#include <QPair>
#include <QQueue>
#include <QString>
#include <QStringList>
#include <QTime>
#include <QUuid>
#include <opencv2/opencv.hpp>

namespace vtk::control
{

using TriggerToAlgosHash = QHash<QString, QList<common::AlgoHandle>>;
using ViewToTriggerCharHash = QHash<QUuid, QString>;
using AIAlgoManager = vtk::algoservice::AIAlgoManager;

enum class RunStatus
{
    Offline,
    Once,
    Cycle
};

struct Task
{
    enum class State
    {
        Init,
        ImageReady,
        RunningAlgo,
        Finished
    };

    Task() : state(State::Init), result(false) {}

    State state;
    AIAlgoManager::AlgoResult algo_result;
    bool result;
};

class TaskGroup
{
public:
    TaskGroup() {}
    TaskGroup(const QList<common::AlgoHandle> &handle_list)
    {
        for (const auto handle : handle_list)
        {
            m_tasks[handle] = Task();
        }
    }

    bool isFinished()
    {
        auto iter = m_tasks.begin();
        while (iter != m_tasks.end())
        {
            if (iter++.value().state != Task::State::Finished)
                return false;
        }
        return true;
    }

    void resetTasksState()
    {
        auto iter = m_tasks.begin();
        while (iter != m_tasks.end())
            iter++.value().state = Task::State::Init;
    }

    bool result()
    {
        auto iter = m_tasks.begin();
        while (iter != m_tasks.end())
        {
            if (!iter++.value().result)
                return false;
        }
        return true;
    }

    Task *getTask(common::AlgoHandle handle)
    {
        const auto &it = m_tasks.find(handle);
        if (it != m_tasks.end())
        {
            return &it.value();
        }
        return nullptr;
    }

    const QHash<common::AlgoHandle, Task> &getTasks() const
    {
        return m_tasks;
    }

    size_t getTaskGroupElapsedTime()
    {
        return m_total_elapsed_time;
    }

    void setTaskGroupElapsedTime(size_t elapsed_time)
    {
        m_total_elapsed_time = elapsed_time;
    }

private:
    QHash<common::AlgoHandle, Task> m_tasks;
    size_t m_total_elapsed_time{0};
};

class RuntimeController : public QObject
{
    Q_OBJECT
public:
    static RuntimeController &getInstance()
    {
        static RuntimeController s_controller;
        return s_controller;
    }

    typedef QMap<QString, QMap<QString, QString>> TriggerMap;
    struct TriggerRuntimeData
    {
        QString current_trigger_level{"L0"};
        QHash<QString, TriggerMap> trigger_hash;  //<device, <trigger_level, <trigger_level_order,trigger_char>>>
    };

    enum class RunMode
    {
        once = 0,  //单次
        cycle      //循环
    };
    ~RuntimeController() = default;

    void init();
    inline Statistics &getStatistics()
    {
        return m_statistics;
    }

    void getViewResult(const QUuid &view_uuid, IntegratedAlgoResult &result, size_t &total_elapsed_time);
    void getViewsPosOfSameSource(const QUuid &view_uuid, QList<int> &pos_list);

signals:
    void signalOnRunStatusChanged(RunStatus status);
    void signalOnAlgoResult(const QUuid &view_uuid, const vtk::common::AlgoResult &algo_result,
                            const QUuid &image_uuid = QUuid(), size_t total_elapsed_time = 0);
    void signalOnStatisticsChanged(const Statistics &statistics);
    void signalCurrentLevelAllTaskHasFinished();

public slots:
    void slotRunOnce();
    void slotRunCycle();
    void slotRunPause();
    void slotResetStatistics();
    void slotConfigChanged();
    void slotRunLocalImage(const QUuid &view_uuid, const cv::Mat &image, const QUuid &image_uuid);

private slots:
    void slotTriggerStart(const QString &trigger_char);
    void slotRunAlgos(const std::string &camera_id, const cv::Mat &cv_image, const QString &trigger_char,
                      const QUuid &image_uuid);

private:
    RuntimeController() = default;
    DISALLOW_COPY_AND_ASSIGN(RuntimeController)

    void clear();
    void reset();
    void resetRuntimeDatas();
    void resetRuntimeDatasOfCameras();
    void resetRuntimeDatasOfAlgos();
    void resetRuntimeDatasOfSources();
    void resetTriggerMode();
    void resetTriggerHash();
    const QString getTriggerCharOfView(const QString &source, int view_pos) const;
    void updateTotalTaskResult(const AIAlgoManager::AlgoResult &result);
    void updateViewTaskResult(const AIAlgoManager::AlgoResult &result);
    bool checkTotalTaskFinshed();
    void updateStatisticsResult();
    void appendResultToLog(const AIAlgoManager::AlgoResult &result);
    void addOneOfTotalTaskGroup();
    bool hasFinishedOfCurrentLevelAllTask();
    bool containsRunLocalImage();

private:
    bool m_is_initialization{false};
    bool m_stop_run{true};
    bool m_is_from_datalist{false};
    ResultStatusType m_total_judge_status{ResultStatusType::notAvailable};
    Statistics m_statistics;
    RunMode m_run_mode{RunMode::once};
    QQueue<TaskGroup> m_total_task_groups;
    QHash<QString, QString> m_used_camera_name_to_camera_id;
    TriggerRuntimeData m_trigger_runtime_data;
    TriggerToAlgosHash m_trigger_to_algos_hash;
    QHash<QUuid, QPair<TaskGroup, QTime>> m_view_task_groups_hash;
    QHash<common::AlgoHandle, QList<QUuid>> m_handle_to_view_uuids_hash;
    QHash<QString, QList<QUuid>> m_trigger_char_to_view_uuids_hash;
    QHash<QString, QList<QUuid>> m_trigger_level_to_view_uuids;
    ViewToTriggerCharHash m_view_uuid_to_trigger_char_hash;
    QHash<QUuid, int> m_view_uuid_to_view_pos_hash;
    QHash<QString, QList<int>> m_source_name_to_views_pos_hash;
    QHash<QUuid, QString> m_view_uuid_to_source_name_hash;
    QHash<QUuid, QList<common::AlgoHandle>> m_view_uuid_to_handles_hash;
};
}  // namespace vtk::control

#endif  // RUNTIMECONTROLLER_H
