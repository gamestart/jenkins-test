/****************************************************************************
 *  @file     applogger.h
 *  @brief    应用程序日志
 *  @author   xiaoyu.liu
 *  @email    xiaoyu.liu@smartmore.com
 *  @version
 *  @date     2021-01-07
 *  Copyright (c) 2021 SmartMore All rights reserved.
 ****************************************************************************/

#ifndef APPLOGGER_H
#define APPLOGGER_H

#include "spdlog/sinks/daily_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"
#include <cstdio>

namespace vtk::applogger
{

class AppLogger
{
public:
    AppLogger(const std::string &name, const std::string &logFileName, spdlog::level::level_enum logLevel);

    ~AppLogger() = default;

    template <typename... Args> void info(const char *fmt, Args... args)
    {
        char buf[mc_buf_size];
        snprintf(buf, sizeof(buf), fmt, args...);
        m_console_logger->info(buf);
        m_file_logger->info(buf);
    }

    template <typename... Args> void warn(const char *fmt, Args... args)
    {
        char buf[mc_buf_size];
        snprintf(buf, sizeof(buf), fmt, args...);
        m_console_logger->warn(buf);
        m_file_logger->warn(buf);
    }

    template <typename... Args> void error(const char *fmt, Args... args)
    {
        char buf[mc_buf_size];
        snprintf(buf, sizeof(buf), fmt, args...);
        m_console_logger->error(buf);
        m_file_logger->error(buf);
    }

    template <typename... Args> void trace(const char *fmt, Args... args)
    {
        char buf[mc_buf_size];
        snprintf(buf, sizeof(buf), fmt, args...);
        m_console_logger->trace(buf);
        m_file_logger->trace(buf);
    }

private:
    static const int mc_buf_size = 256;

    std::shared_ptr<spdlog::logger> m_console_logger;
    std::shared_ptr<spdlog::logger> m_file_logger;
};

extern std::shared_ptr<AppLogger> g_app_logger;

void initializeLog();

/**
 * 日志宏
 * 分别对应info，warn，error，trace等级。
 * tag 优先使用类名，非类函数使用namespace名或函数名，不加引号。
 */

#define LOGI(tag, fmt, ...) vtk::applogger::g_app_logger->info("[%s] [I] " fmt, #tag, ##__VA_ARGS__)

#define LOGW(tag, fmt, ...) vtk::applogger::g_app_logger->warn("[%s] [W] " fmt, #tag, ##__VA_ARGS__)

#define LOGE(tag, fmt, ...) vtk::applogger::g_app_logger->error("[%s] [E] " fmt, #tag, ##__VA_ARGS__)

#define LOGT(tag, fmt, ...) vtk::applogger::g_app_logger->trace("[%s] [T] " fmt, #tag, ##__VA_ARGS__)

}  // namespace vtk::applogger

#endif  // APPLOGGER_H
