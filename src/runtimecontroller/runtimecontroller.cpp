/****************************************************************************
 *  @file     runtimecontroller.cpp
 *  @brief    vtk运行控制类
 *  @author   xiaoyu.liu
 *  @email    xiaoyu.liu@smartmore.com
 *  @version
 *  @date     2020-12-25
 *  Copyright (c) 2020 SmartMore All rights reserved.
 ****************************************************************************/
#include "runtimecontroller.h"
#include "cameraservice/cameramanager.h"
#include "configdatamanager/configmanager.h"
#include "imagesaver/asyncimagesaver.h"
#include "logservice/logservice.h"
#include <QDateTime>
#include <QDebug>
#include <QFileInfo>
#include <QTextCodec>
#include <QTimer>
#include <memory>

#define TRIGGERLEVELMAX 4  //触发层级上限值

namespace vtk::control
{
using ConfigManager = vtk::config::ConfigManager;
using CameraManager = vtk::cameraservice::CameraManager;
using Message = vtk::common::Message;
using AlgoType = vtk::common::AlgoType;
using AsyncImageSaver = vtk::imagesave::AsyncImageSaver;
using TriggerMode = vtk::config::TriggerMode;
using namespace logservice;

void RuntimeController::init()
{
    if (!m_is_initialization)
    {
        m_statistics.reset();

        //连接算法信号
        auto &algo_manager = AIAlgoManager::getInstance();
        connect(&algo_manager, &AIAlgoManager::signalUpdateResult, [&](const AIAlgoManager::AlgoResult &result) {
            LOGI(RuntimeController, "Accept algo's result");
            if (m_stop_run)
                return;

            updateTotalTaskResult(result);
            updateViewTaskResult(result);
            appendResultToLog(result);
            if (checkTotalTaskFinshed())
            {
                m_total_task_groups.dequeue();
                updateStatisticsResult();
            }
        });

        //连接相机信号
        auto &camera_manager = CameraManager::getInstance();
        connect(&camera_manager, &CameraManager::signalSendImage, this, &RuntimeController::slotRunAlgos);
        connect(&camera_manager, &CameraManager::signalTriggerStart, this, &RuntimeController::slotTriggerStart);

        //连接配置信号
        connect(&ConfigManager::getInstance(), &ConfigManager::signalConfigChanged, this,
                &RuntimeController::slotConfigChanged);
        connect(&ConfigManager::getInstance(), &ConfigManager::signalTriggerModeChanged, this,
                [&] { resetTriggerMode(); });

        connect(this, &RuntimeController::signalCurrentLevelAllTaskHasFinished, this, [&] {
            if (m_run_mode == RunMode::cycle)
            {
                const auto trigger_mode = ConfigManager::getInstance().getTriggerMode();
                if (trigger_mode == vtk::config::TriggerMode::hardTrigger)
                    slotRunOnce();
                else
                    QTimer::singleShot(10, this, [&] { slotRunOnce(); });
            }
            else
            {
                CameraManager::getInstance().stopCamerasCapture();
            }
        });

        reset();
        m_is_initialization = true;
    }
}

void RuntimeController::clear()
{
    m_used_camera_name_to_camera_id.clear();
    m_trigger_to_algos_hash.clear();
    m_view_task_groups_hash.clear();
    m_handle_to_view_uuids_hash.clear();
    m_trigger_level_to_view_uuids.clear();
    m_trigger_char_to_view_uuids_hash.clear();
    m_view_uuid_to_trigger_char_hash.clear();
    m_view_uuid_to_view_pos_hash.clear();
    m_view_uuid_to_source_name_hash.clear();
    m_source_name_to_views_pos_hash.clear();
    m_view_uuid_to_handles_hash.clear();
    m_total_task_groups.clear();
    auto &algo_manager = AIAlgoManager::getInstance();
    algo_manager.clearAlgos();
}

void RuntimeController::reset()
{
    AsyncImageSaver::startTasks();
    clear();
    resetRuntimeDatas();
}

void RuntimeController::resetRuntimeDatas()
{
    resetRuntimeDatasOfCameras();
    resetRuntimeDatasOfAlgos();
    resetRuntimeDatasOfSources();
    resetTriggerMode();
    resetTriggerHash();
}

void RuntimeController::resetRuntimeDatasOfCameras()
{
    //相机运行相关数据的初始化
    const auto &config_manager = ConfigManager::getInstance();
    const auto &camera_data_list = config_manager.getCameraConfigData();
    const auto &view_data_list = config_manager.getViewConfigData();
    const auto &source_data_list = config_manager.getSourceConfigData();
    auto &cameras_manager = CameraManager::getInstance();
    int view_pos{0};

    auto add_vmcamera = [&](const QString &id, const QUuid &view_uuid) {
        auto cap = cameras_manager.getCameraDevice(id.toStdString());
        if (!cap)
        {
            CameraManager::getInstance().addVMCamera(view_uuid, id.toStdString());
        }
        else
        {
            if (cap->getVMCameraUuid() != view_uuid)
            {
                cameras_manager.deleteVMCameras(id.toStdString());
                CameraManager::getInstance().addVMCamera(view_uuid, id.toStdString());
            }
        }
    };

    foreach (const auto &view_data, view_data_list)
    {
        if (view_data.source == "File")
        {
            const auto &camera_name = QString("Local%1").arg(view_pos);
            const auto &camera_id = QString("VMCamera%1").arg(view_pos + 1);
            m_used_camera_name_to_camera_id[camera_name] = camera_id;
            m_used_camera_name_to_camera_id.insert(camera_name, camera_id);
            add_vmcamera(camera_id, view_data.uuid);
        }
        else
        {
            auto result = std::any_of(source_data_list.begin(), source_data_list.end(),
                                      [&](const vtk::config::SourceConfigItem &source_data) {
                                          if (view_data.source == source_data.source_name)
                                          {
                                              m_used_camera_name_to_camera_id[source_data.source] = "";
                                              return true;
                                          }
                                          return false;
                                      });
        }
        view_pos++;
    }

    for (const auto &camera_data : camera_data_list)
    {
        if (m_used_camera_name_to_camera_id.contains(camera_data.name))
            m_used_camera_name_to_camera_id[camera_data.name] = camera_data.id;
    }
}

void RuntimeController::resetRuntimeDatasOfAlgos()
{
    //算法运行相关数据的初始化
    const auto &config_manager = ConfigManager::getInstance();
    auto &algo_manager = AIAlgoManager::getInstance();
    const auto &view_data_list = config_manager.getViewConfigData();
    const auto &algo_config_data = config_manager.getAlgoConfigData();

    int view_pos{0};
    foreach (const auto &view_data, view_data_list)
    {
        QList<common::AlgoHandle> handles;
        for (const auto &algo : algo_config_data.items)
        {
            for (const auto &view_algo : view_data.algo)
            {
                if (view_algo == algo.name)
                {
                    auto handle = algo_manager.initAlgo(algo.type, algo.model_file, algo_config_data.gpu_mode);
                    m_handle_to_view_uuids_hash[handle].append(view_data.uuid);
                    handles.append(handle);
                    break;
                }
            }
        }
        if (view_data.algo.isEmpty())
        {
            auto handle = algo_manager.initAlgo(AlgoType::noType);
            m_handle_to_view_uuids_hash[handle].append(view_data.uuid);
            handles.append(handle);
        }

        const auto &trigger_char = getTriggerCharOfView(view_data.source, view_pos);
        for (const auto handle : handles)
        {
            if (!m_trigger_to_algos_hash[trigger_char].contains(handle))
                m_trigger_to_algos_hash[trigger_char].append(handle);
        }
        m_view_uuid_to_handles_hash[view_data.uuid] = handles;
        m_view_task_groups_hash[view_data.uuid] = std::move(QPair(TaskGroup(handles), QTime()));
        m_view_uuid_to_view_pos_hash[view_data.uuid] = view_pos;
        view_pos++;
    }
}

void RuntimeController::resetRuntimeDatasOfSources()
{
    //数据源在运行时相关数据的初始化
    const auto &config_manager = ConfigManager::getInstance();
    const auto &view_data_list = config_manager.getViewConfigData();
    const auto &source_data_list = config_manager.getSourceConfigData();

    int view_pos{0};
    foreach (const auto &view_data, view_data_list)
    {
        auto result = std::any_of(
            source_data_list.begin(), source_data_list.end(), [&](const vtk::config::SourceConfigItem &source_data) {
                if ((view_data.source == "File" && source_data.source_name == QString("LocalSource%1").arg(view_pos)) ||
                    view_data.source == source_data.source_name)
                {
                    m_trigger_level_to_view_uuids[source_data.trigger_level].append(view_data.uuid);
                    m_trigger_char_to_view_uuids_hash[source_data.trigger_char].append(view_data.uuid);
                    m_view_uuid_to_trigger_char_hash[view_data.uuid] = source_data.trigger_char;
                    m_source_name_to_views_pos_hash[source_data.source_name].append(view_pos);
                    m_view_uuid_to_source_name_hash[view_data.uuid] = source_data.source_name;
                    return true;
                }
                return false;
            });
        view_pos++;
    }
}

void RuntimeController::resetTriggerMode()
{
    const auto trigger_mode = ConfigManager::getInstance().getTriggerMode();
    CameraManager::getInstance().setCamerasTriggerMode(static_cast<int>(trigger_mode));
}

const QString RuntimeController::getTriggerCharOfView(const QString &source, int view_pos) const
{
    QString trigger_char;
    const auto &source_data_list = ConfigManager::getInstance().getSourceConfigData();
    auto result = std::any_of(
        source_data_list.begin(), source_data_list.end(), [&](const vtk::config::SourceConfigItem &source_data) {
            if ((source == "File" && source_data.source_name == QString("LocalSource%1").arg(view_pos)) ||
                source == source_data.source_name)
            {
                trigger_char = source_data.trigger_char;
                return true;
            }
            return false;
        });
    return trigger_char;
}

void RuntimeController::updateTotalTaskResult(const AIAlgoManager::AlgoResult &result)
{
    //更新任务状态和结果
    if (m_is_from_datalist || m_total_task_groups.isEmpty())
        return;

    auto task = m_total_task_groups.first().getTask(result.algo_handle);
    if (task && task->state == Task::State::RunningAlgo)
    {
        task->state = Task::State::Finished;
        task->result = (result.result_status != common::ResultStatusType::ng);
        if (result.result_status == common::ResultStatusType::ng)
        {
            m_total_judge_status = common::ResultStatusType::ng;
        }
        else if (m_total_judge_status == ResultStatusType::notAvailable && !result.image.empty())
        {
            m_total_judge_status = common::ResultStatusType::ok;
        }
    }
}

void RuntimeController::updateViewTaskResult(const AIAlgoManager::AlgoResult &result)
{
    if (!m_handle_to_view_uuids_hash.contains(result.algo_handle))
        return;

    const auto &view_uuids = m_handle_to_view_uuids_hash[result.algo_handle];
    for (const auto &view_uuid : view_uuids)
    {
        auto view_task = m_view_task_groups_hash[view_uuid].first.getTask(result.algo_handle);
        if (view_task && view_task->state == Task::State::RunningAlgo)
        {
            view_task->state = Task::State::Finished;
            if (!m_is_from_datalist)
                AsyncImageSaver::saveResultImages(m_view_uuid_to_view_pos_hash[view_uuid], result,
                                                  !result.image_uuid.isNull());
            view_task->algo_result = result;  //把结果全部存起来，提供给外部模块主动访问使用
        }

        if (!m_view_task_groups_hash[view_uuid].first.isFinished())
            continue;

        auto total_elapsed_time = m_view_task_groups_hash[view_uuid].second.elapsed();
        m_view_task_groups_hash[view_uuid].first.setTaskGroupElapsedTime(total_elapsed_time);
        emit signalOnAlgoResult(view_uuid, result, result.image_uuid, total_elapsed_time);

        if (!m_is_from_datalist)
        {
            if (!hasFinishedOfCurrentLevelAllTask())
                continue;

            LOGI(RuntimeController, "Current level all task has finished");
            if (m_run_mode == RunMode::cycle)
                emit signalCurrentLevelAllTaskHasFinished();
            else
            {
                emit signalOnRunStatusChanged(RunStatus::Offline);
                m_stop_run = true;
            }
        }
        else
        {
            m_view_task_groups_hash[view_uuid].first.resetTasksState();
            m_is_from_datalist = false;
            m_stop_run = true;
        }
    }
}

bool RuntimeController::checkTotalTaskFinshed()
{
    if (m_is_from_datalist || m_total_task_groups.isEmpty())
        return false;

    return m_total_task_groups.front().isFinished();
}

void RuntimeController::updateStatisticsResult()
{
    if (m_total_judge_status == ResultStatusType::notAvailable)
        return;

    m_statistics.addResult(m_total_judge_status);
    emit signalOnStatisticsChanged(m_statistics);
    m_total_judge_status = ResultStatusType::notAvailable;
}

void RuntimeController::appendResultToLog(const AIAlgoManager::AlgoResult &result)
{
    //算法结果记录日志
    QString labs;
    for (const auto &lab : result.lab_names)
        labs.append(QString::fromStdString(lab) + " ");

    QString tip = labs;
    switch (result.result_status)
    {
        case AIAlgoManager::ResultStatusType::ok:
            tip += "OK";
            break;
        case AIAlgoManager::ResultStatusType::ng:
            tip += "NG";
            break;
        default:
            tip += "NONE";
            break;
    }
    if (!result.image.empty() && !m_is_from_datalist)
        Logger::getInstance().appendLog({LogLevel::Info, LogSource::AI}, tip);
}

void RuntimeController::getViewResult(const QUuid &view_uuid, IntegratedAlgoResult &result, size_t &total_elapsed_time)
{
    if (m_view_task_groups_hash.contains(view_uuid) && m_view_uuid_to_handles_hash.contains(view_uuid))
    {
        for (const auto handle : m_view_uuid_to_handles_hash[view_uuid])
        {
            auto original_lab_names = result.lab_names;
            auto single_algo_result = m_view_task_groups_hash[view_uuid].first.getTask(handle)->algo_result;
            result.elapsed_time = single_algo_result.elapsed_time;
            result.image = single_algo_result.image;
            result.ai_images.emplace_back(single_algo_result.ai_image);
            std::string new_lab_names{""};
            for (const auto &lab : single_algo_result.lab_names)
                new_lab_names += (lab + " ");
            original_lab_names.emplace_back(new_lab_names);
            result.lab_names = original_lab_names;
            switch (single_algo_result.result_status)
            {
                case ResultStatusType::ng:
                    result.result_status = ResultStatusType::ng;
                    break;
                case ResultStatusType::ok:
                    if (result.result_status != ResultStatusType::ng)
                        result.result_status = ResultStatusType::ok;
                    break;
                default:
                    break;
            }
        }
        total_elapsed_time = m_view_task_groups_hash[view_uuid].first.getTaskGroupElapsedTime();
    }
}

void RuntimeController::getViewsPosOfSameSource(const QUuid &view_uuid, QList<int> &pos_list)
{
    if (m_view_uuid_to_source_name_hash.contains(view_uuid))
        pos_list = m_source_name_to_views_pos_hash[m_view_uuid_to_source_name_hash[view_uuid]];
}

void RuntimeController::slotRunOnce()
{
    const auto trigger_mode = ConfigManager::getInstance().getTriggerMode();
    if (m_is_from_datalist || (trigger_mode == vtk::config::TriggerMode::hardTrigger && m_run_mode == RunMode::once))
    {
        emit signalOnRunStatusChanged(RunStatus::Offline);
        return;
    }
    m_stop_run = false;
    LOGI(RuntimeController, "Run once start");
    if (m_run_mode == RunMode::once)
        emit signalOnRunStatusChanged(RunStatus::Once);
    auto &current_level = m_trigger_runtime_data.current_trigger_level;
    const auto &trigger_hash = m_trigger_runtime_data.trigger_hash;
    bool has_trigger{false};
    while (!has_trigger)
    {
        if (current_level == "L0")
            addOneOfTotalTaskGroup();
        auto iter = trigger_hash.begin();
        while (iter != trigger_hash.end())
        {
            if (m_total_task_groups.isEmpty())
                break;
            const auto &camera_id = iter.key();
            const auto &trigger_map = iter.value();
            auto iter1 = trigger_map.begin();
            while (iter1 != trigger_map.end())
            {
                if (iter1.key() == current_level)
                {
                    const auto &trigger_level_map = iter1.value();
                    auto iter2 = trigger_level_map.begin();
                    QList<QString> trigger_char_list;
                    QString trigger_chars;
                    while (iter2 != trigger_level_map.end())
                    {
                        trigger_chars += iter2.value() + " ";
                        trigger_char_list.append(iter2++.value());
                    }
                    LOGI(RuntimeController, "Current level %s contains tirgger chars is %s!",
                         current_level.toLatin1().data(), trigger_chars.toLatin1().data());
                    bool is_ok =
                        CameraManager::getInstance().startCameraCapture(camera_id.toStdString(), trigger_char_list);
                    if (!is_ok)
                        slotRunPause();
                    has_trigger = true;
                    break;
                }
                iter1++;
            }
            iter++;
        }

        if (!has_trigger)
        {
            LOGI(RuntimeController, "%s not trigger!", current_level.toLatin1().data());
            auto level_value = current_level.right(1).toInt();
            current_level = (level_value == TRIGGERLEVELMAX ? "L0" : QString("L%1").arg(++level_value));
        }
        else
            LOGI(RuntimeController, "%s has triggered!", current_level.toLatin1().data());
    }
}

void RuntimeController::slotRunCycle()
{
    const auto trigger_mode = ConfigManager::getInstance().getTriggerMode();
    if (m_is_from_datalist || (trigger_mode == vtk::config::TriggerMode::hardTrigger && containsRunLocalImage()))
    {
        if (!m_is_from_datalist)
        {
            Message::postWarningMessage(tr("When the configuration solution is in hard trigger mode, "
                                           "local pictures cannot be run, please check whether there "
                                           "are local files in the data source in the view settings!"));
        }
        emit signalOnRunStatusChanged(RunStatus::Offline);
        return;
    }
    LOGI(RuntimeController, "Run cycle start");
    m_run_mode = RunMode::cycle;
    emit signalOnRunStatusChanged(RunStatus::Cycle);
    slotRunOnce();
}

void RuntimeController::slotRunPause()
{
    m_stop_run = true;
    emit signalOnRunStatusChanged(RunStatus::Offline);
    for (const auto &uuid : m_trigger_level_to_view_uuids[m_trigger_runtime_data.current_trigger_level])
        m_view_task_groups_hash[uuid].first.resetTasksState();
    if (!m_total_task_groups.isEmpty())
        m_total_task_groups.clear();
    CameraManager::getInstance().stopCamerasCapture();
    m_run_mode = RunMode::once;
}

void RuntimeController::slotResetStatistics()
{
    m_statistics.reset();
}

void RuntimeController::slotConfigChanged()
{
    reset();
}

void RuntimeController::slotTriggerStart(const QString &trigger_char)
{
    if (m_trigger_char_to_view_uuids_hash.contains(trigger_char))
    {
        const auto &view_uuid_list = m_trigger_char_to_view_uuids_hash[trigger_char];
        for (const auto &view_uuid : view_uuid_list)
        {
            m_view_task_groups_hash[view_uuid].second.restart();
        }
        const auto &source_data_list = ConfigManager::getInstance().getSourceConfigData();
        for (const auto &source_data : source_data_list)
        {
            if (source_data.trigger_char == trigger_char && !source_data.source.contains("Local"))
                Logger::getInstance().appendLog({LogLevel::Info, LogSource::Camera},
                                                QString("%1 is start capture!").arg(source_data.source));
        }
    }
}

void RuntimeController::slotRunLocalImage(const QUuid &view_uuid, const cv::Mat &image, const QUuid &image_uuid)
{
    if (m_run_mode == RunMode::cycle || !m_total_task_groups.isEmpty() || m_is_from_datalist)
        return;

    m_is_from_datalist = true;
    m_stop_run = false;
    const auto &trigger_char = m_view_uuid_to_trigger_char_hash[view_uuid];
    m_view_task_groups_hash[view_uuid].second.restart();
    const auto &tasks = m_view_task_groups_hash[view_uuid].first.getTasks();
    for (const auto &algo_handle : tasks.keys())
    {
        m_view_task_groups_hash[view_uuid].first.getTask(algo_handle)->state = Task::State::RunningAlgo;
        AIAlgoManager::getInstance().runAlgo(algo_handle, trigger_char, image, image_uuid);
    }
}

void RuntimeController::slotRunAlgos(const std::string &camera_id, const cv::Mat &cv_image, const QString &trigger_char,
                                     const QUuid &image_uuid)
{
    LOGI(RuntimeController, "Run algos start!");
    if (m_stop_run)
        return;
    if (m_trigger_to_algos_hash.contains(trigger_char))
    {
        for (auto algo_handle : m_trigger_to_algos_hash[trigger_char])
        {
            if (m_total_task_groups.isEmpty())
            {
                LOGW(RuntimeController, "Total task is empty!");
                return;
            }
            auto total_task = m_total_task_groups.first().getTask(algo_handle);
            if (m_handle_to_view_uuids_hash.contains(algo_handle))
            {
                const auto &view_uuids = m_handle_to_view_uuids_hash[algo_handle];
                for (const auto &view_uuid : view_uuids)
                {
                    auto view_task = m_view_task_groups_hash[view_uuid].first.getTask(algo_handle);
                    if (view_task &&
                        (view_task->state == Task::State::Init || view_task->state == Task::State::ImageReady))
                    {
                        view_task->state = Task::State::RunningAlgo;
                    }
                }
            }

            if (total_task && (total_task->state == Task::State::Init || total_task->state == Task::State::ImageReady))
            {
                total_task->state = Task::State::RunningAlgo;
                LOGI(RuntimeController, "algo handle %d ready Run", algo_handle);
                AIAlgoManager::getInstance().runAlgo(algo_handle, trigger_char, cv_image, image_uuid);
            }
        }
    }
    else
        LOGW(RuntimeController, "Trigger char invalid!");
}

void RuntimeController::resetTriggerHash()
{
    m_trigger_runtime_data.trigger_hash.clear();
    const auto &config_manager = ConfigManager::getInstance();
    const auto &view_data_list = config_manager.getViewConfigData();
    const auto &source_data_list = config_manager.getSourceConfigData();

    auto update_trigger_hash = [&](const vtk::config::SourceConfigItem &source_data) {
        m_trigger_runtime_data.trigger_hash[m_used_camera_name_to_camera_id.value(source_data.source)]
                                           [source_data.trigger_level][source_data.trigger_level_order] =
            source_data.trigger_char;
    };

    int view_pos{0};
    foreach (const auto &view_data, view_data_list)
    {
        if (view_data.source == "File")
            update_trigger_hash(source_data_list.at(view_pos));
        else
        {
            foreach (const auto &source_data, source_data_list)
            {
                if (view_data.source == source_data.source_name)
                {
                    update_trigger_hash(source_data);
                    break;
                }
            }
        }
        view_pos++;
    }

    //初始化就绪状态
    m_trigger_runtime_data.current_trigger_level = "L0";
}

void RuntimeController::addOneOfTotalTaskGroup()
{
    //加入总的任务队列(即所有视窗的handle集合)
    QList<common::AlgoHandle> handle_list;
    auto iter = m_view_task_groups_hash.begin();
    while (iter != m_view_task_groups_hash.end())
    {
        auto &tasks = iter.value().first.getTasks();
        auto iter1 = tasks.begin();
        while (iter1 != tasks.end())
            handle_list.append(iter1++.key());
        iter++;
    }
    m_total_task_groups.enqueue(TaskGroup(handle_list));
}

bool RuntimeController::hasFinishedOfCurrentLevelAllTask()
{
    auto &current_level = m_trigger_runtime_data.current_trigger_level;
    bool has_finished_of_current_level_all_tast{true};
    for (const auto &uuid : m_trigger_level_to_view_uuids[current_level])
    {
        if (m_view_task_groups_hash[uuid].first.isFinished())
            continue;
        has_finished_of_current_level_all_tast = false;
    }
    if (has_finished_of_current_level_all_tast)
    {
        for (const auto &uuid : m_trigger_level_to_view_uuids[current_level])
            m_view_task_groups_hash[uuid].first.resetTasksState();
        auto level_value = current_level.right(1).toInt();
        current_level = (level_value == TRIGGERLEVELMAX ? "L0" : QString("L%1").arg(++level_value));
    }
    return has_finished_of_current_level_all_tast;
}

bool RuntimeController::containsRunLocalImage()
{
    const auto &view_data_list = ConfigManager::getInstance().getViewConfigData();
    auto result =
        std::any_of(view_data_list.begin(), view_data_list.end(), [&](const vtk::config::ViewConfigItem &view_data) {
            if (view_data.source == "File")
            {
                return true;
            }
            return false;
        });
    return result;
}

}  // namespace vtk::control
