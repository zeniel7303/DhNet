#include "stdafx.h"
#include "MetricsAggregator.h"

#include <algorithm>
#include <numeric>

MetricsAggregator& MetricsAggregator::Instance()
{
    static MetricsAggregator instance;
    return instance;
}

void MetricsAggregator::RecordRtt(double _ms)
{
    WRITE_LOCK;
    m_rttSamplesMs.push_back(_ms);
    m_successCount++;
}

void MetricsAggregator::RecordTimeout()
{
    WRITE_LOCK;
    m_timeoutCount++;
}

void MetricsAggregator::RecordStuck()
{
    WRITE_LOCK;
    m_stuckCount++;
}

void MetricsAggregator::RecordConnection()
{
    m_totalConnections.fetch_add(1, std::memory_order_relaxed);
}

uint64_t MetricsAggregator::GetTotalConnections() const
{
    return m_totalConnections.load(std::memory_order_relaxed);
}

namespace
{
    double Percentile(std::vector<double>& _sorted, double _p)
    {
        if (_sorted.empty())
            return 0.0;

        size_t idx = static_cast<size_t>(_p * (_sorted.size() - 1));
        return _sorted[idx];
    }
}

MetricsSnapshot MetricsAggregator::GetSnapshotAndReset()
{
    std::vector<double> samples;
    MetricsSnapshot snapshot;

    {
        WRITE_LOCK;
        samples = std::move(m_rttSamplesMs);
        snapshot.successCount = m_successCount;
        snapshot.timeoutCount = m_timeoutCount;
        snapshot.stuckCount = m_stuckCount;
        m_successCount = 0;
        m_timeoutCount = 0;
        m_stuckCount = 0;
    }

    snapshot.sampleCount = samples.size();
    if (!samples.empty())
    {
        std::sort(samples.begin(), samples.end());
        snapshot.minMs = samples.front();
        snapshot.maxMs = samples.back();
        snapshot.avgMs = std::accumulate(samples.begin(), samples.end(), 0.0) / samples.size();
        snapshot.p50Ms = Percentile(samples, 0.50);
        snapshot.p95Ms = Percentile(samples, 0.95);
        snapshot.p99Ms = Percentile(samples, 0.99);
    }

    return snapshot;
}
