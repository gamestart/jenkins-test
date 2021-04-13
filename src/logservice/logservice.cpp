/****************************************************************************
 *  @file     logger.cpp
 *  @brief    运行日志
 *  @author   xiaoyu.liu
 *  @email    xiaoyu.liu@smartmore.com
 *  @version
 *  @date     2020-12-28
 *  Copyright (c) 2020 SmartMore All rights reserved.
 ****************************************************************************/

#include "logservice.h"
#include "common/vtkcommon.h"
#include <QDateTime>
#include <QFile>
#include <QTextStream>
#include <mutex>

namespace vtk::logservice
{

using Message = vtk::common::Message;

QString Logger::getDefaultLogName()
{
    QString log_name = QDateTime::currentDateTime().toString("yyyy-MM-dd-hh-mm-ss");
    log_name.append("_Smore_Log_");
    log_name.append(m_filter.toString());
    log_name.append(".txt");
    return log_name;
}

void Logger::appendLog(LogType type, QString content)
{
    m_tlogs.push_back({QDateTime::currentDateTime().toString("hh:mm:ss"), type, std::move(content)});

    if (m_filter.contains(type))
    {
        m_clogs.push_back(&m_tlogs.back());
        emit signalAppendLog(m_tlogs.back());
    }
}

void Logger::setFilter(LogFilter filter)
{
    m_filter = filter;
    m_clogs.clear();
    for (auto &it : m_tlogs)
    {
        if (m_filter.contains(it.type))
        {
            m_clogs.push_back(&it);
        }
    }

    emit signalReloadLog(m_clogs);
}

QString LogFilter::toString()
{
    if (m_filter_mode == None)
        return QString("All");
    if (m_filter_mode == Level)
    {
        switch (m_type.level)
        {
            case LogLevel::Info:
                return QString("Info");
            case LogLevel::Warning:
                return QString("Warning");
            case LogLevel::Error:
                return QString("Error");
        }
    }
    if (m_filter_mode == Source)
    {
        switch (m_type.source)
        {
            case LogSource::AI:
                return QString("AI");
            case LogSource::Camera:
                return QString("Camera");
            case LogSource::Others:
                return QString("Others");
        }
    }
    return QString("");
}

}  // namespace vtk::logservice
