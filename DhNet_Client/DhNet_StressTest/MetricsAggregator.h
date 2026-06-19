#pragma once
#include <vector>
#include <cstdint>

// 모든 세션의 RTT/성공/타임아웃을 모으는 전역 스레드세이프 집계기.
// GetSnapshotAndReset()을 호출한 시점까지의 구간 통계를 반환하고 내부 상태를 비운다 —
// Phase 5.3의 주기적 콘솔 리포트가 매 10초 간격으로 "그 구간"의 통계를 찍는 데 맞춰진 모델.
struct MetricsSnapshot
{
    uint64_t successCount = 0;
    uint64_t timeoutCount = 0;
    size_t sampleCount = 0;
    double minMs = 0.0;
    double maxMs = 0.0;
    double avgMs = 0.0;
    double p50Ms = 0.0;
    double p95Ms = 0.0;
    double p99Ms = 0.0;
};

class MetricsAggregator
{
    USE_LOCK;
    std::vector<double> m_rttSamplesMs;
    uint64_t m_successCount = 0;
    uint64_t m_timeoutCount = 0;

public:
    static MetricsAggregator& Instance();

    void RecordRtt(double _ms);
    void RecordTimeout();

    MetricsSnapshot GetSnapshotAndReset();
};
