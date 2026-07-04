#pragma once
#include <vector>
#include <cstdint>
#include <atomic>

// 모든 세션의 RTT/성공/타임아웃을 모으는 전역 스레드세이프 집계기.
// GetSnapshotAndReset()을 호출한 시점까지의 구간 통계를 반환하고 내부 상태를 비운다 —
// Phase 5.3의 주기적 콘솔 리포트가 매 10초 간격으로 "그 구간"의 통계를 찍는 데 맞춰진 모델.
struct MetricsSnapshot
{
    uint64_t successCount = 0;
    uint64_t timeoutCount = 0;
    uint64_t stuckCount = 0; // 이 구간에서 새로 Stuck 전환된 세션 수 (핑 타임아웃과는 다른 실패 유형)
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
    uint64_t m_stuckCount = 0;
    std::atomic<uint64_t> m_totalConnections{ 0 }; // 프로세스 시작 이후 누적 — GetSnapshotAndReset으로 리셋되지 않음

public:
    static MetricsAggregator& Instance();

    void RecordRtt(double _ms);
    void RecordTimeout();
    void RecordStuck();
    void RecordConnection();

    uint64_t GetTotalConnections() const;

    MetricsSnapshot GetSnapshotAndReset();
};
