/****************************************************************************
 *  @file     applogger.cpp
 *  @brief    应用程序日志
 *  @author   xiaoyu.liu
 *  @email    xiaoyu.liu@smartmore.com
 *  @version
 *  @date     2021-01-08
 *  Copyright (c) 2021 SmartMore All rights reserved.
 ****************************************************************************/

#include "applogger.h"
#include "spdlog/cfg/env.h"

namespace vtk::applogger
{
std::shared_ptr<AppLogger> g_app_logger;

void initializeLog()
{
    spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] %v");
    g_app_logger = std::make_shared<AppLogger>("smore_vtk", "logs/smore_vtk.log", spdlog::level::info);
    spdlog::cfg::load_env_levels();
    LOGI(applogger, "log is initialized");
}

AppLogger::AppLogger(const std::string &name, const std::string &logFileName, spdlog::level::level_enum logLevel)
{
    std::string conId = name;
    std::string fileId = "file_" + name;
    std::string correctFileName = logFileName;
    for (auto &ch : correctFileName)
    {
        switch (ch)
        {
            case ':':
            case '|':
            case '>':
            case '<':
            case '"':
            case '*':
            case '?':
                ch = ' ';
            default:
                break;
        }
    }
    if (spdlog::get(conId) == nullptr)
    {
        m_console_logger = spdlog::stdout_color_mt(conId);
        m_console_logger->set_level(logLevel);

        m_file_logger = spdlog::daily_logger_mt(fileId, correctFileName, 0, 0);
        m_file_logger->set_level(logLevel);
        m_file_logger->flush_on(logLevel);
    }
    else
    {
        m_console_logger = spdlog::get(conId);
        m_file_logger = spdlog::get(fileId);
    }
}

}  // namespace vtk::applogger
