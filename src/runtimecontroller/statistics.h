#ifndef STATISTICS_H
#define STATISTICS_H

#include "common/vtkcommon.h"

namespace vtk::control
{
using namespace vtk::common;
class Statistics
{

public:
    Statistics()
    {
        reset();
    }

    void reset()
    {
        m_total_counts = 0;
        m_ng_counts = 0;
        m_ok_counts = 0;
        m_pass_percent = 0.0000f;
    }

    void addResult(ResultStatusType type)
    {
        switch (type)
        {
            case ResultStatusType::ok:
                m_ok_counts++;
                break;
            case ResultStatusType::ng:
                m_ng_counts++;
                break;
            default:
                break;
        }
        m_total_counts++;
        m_pass_percent = m_ok_counts * 100.0f / m_total_counts;
    }

    size_t totalCounts() const
    {
        return m_total_counts;
    }

    size_t ngCounts() const
    {
        return m_ng_counts;
    }

    size_t okCounts() const
    {
        return m_ok_counts;
    }

    float passPercent() const
    {
        return m_pass_percent;
    }

private:
    size_t m_total_counts{0};
    size_t m_ng_counts{0};
    size_t m_ok_counts{0};
    float m_pass_percent{0.0000f};
};

}  // namespace vtk::control

#endif  // STATISTICS_H
