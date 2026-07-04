#pragma once
#include <vector>
#include <mutex>
#include <chrono>

#include "MetricsAggregator.h"

// 10초 주기 리포트 히스토리 축적, 콘솔 요약 출력, 종료 시 CSV 파일 생성을 담당.
// MetricsAggregator와 달리 락 경합이 거의 없어(메인 스레드가 10초마다 쓰고,
// 콘솔 Ctrl+C 핸들러 스레드가 종료 시점에 드물게 접근) std::mutex로 충분함.
// 단, WriteFinalReport()가 메인 스레드의 다음 ReportInterval() 호출과 실제로
// 동시에 실행될 수 있음(메인 루프는 ExitProcess가 불릴 때까지 계속 돌아감) —
// std::mutex가 이 경합 자체는 안전하게 막아주지만 "종료 시 1회"라고 순서가
// 보장되는 것은 아니다.
struct IntervalReport
{
    std::chrono::system_clock::time_point timestamp;
    uint64_t totalConnections = 0;
    MetricsSnapshot snapshot;
};

class ReportManager
{
    std::mutex m_mutex;
    std::vector<IntervalReport> m_history;

public:
    static ReportManager& Instance();

    // MetricsAggregator의 현재 구간 스냅샷을 콘솔에 출력하고 히스토리에 적재한다.
    void ReportInterval();

    // 누적된 히스토리를 CSV 파일로 저장한다 (프로세스 종료 시 1회 호출).
    void WriteFinalReport();
};
