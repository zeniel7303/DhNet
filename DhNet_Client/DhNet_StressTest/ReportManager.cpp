#include "stdafx.h"
#include "ReportManager.h"

#include <fstream>
#include <iomanip>
#include <sstream>
#include <ctime>

ReportManager& ReportManager::Instance()
{
    static ReportManager instance;
    return instance;
}

namespace
{
    std::wstring FormatTimestamp(std::chrono::system_clock::time_point _tp, const wchar_t* _fmt)
    {
        std::time_t t = std::chrono::system_clock::to_time_t(_tp);
        std::tm tm{};
        localtime_s(&tm, &t);
        std::wstringstream ss;
        ss << std::put_time(&tm, _fmt);
        return ss.str();
    }

    // 콘솔 출력은 LOG_*(spdlog)를 거치므로 std::cout/wcout의 스트림 orientation 문제와
    // 무관하다 — CSV 파일(std::wofstream, 별도 스트림)에서만 wide 포맷을 계속 사용한다.
    std::string FormatTimestampNarrow(std::chrono::system_clock::time_point _tp, const char* _fmt)
    {
        std::time_t t = std::chrono::system_clock::to_time_t(_tp);
        std::tm tm{};
        localtime_s(&tm, &t);
        std::ostringstream ss;
        ss << std::put_time(&tm, _fmt);
        return ss.str();
    }
}

void ReportManager::ReportInterval()
{
    auto snap = MetricsAggregator::Instance().GetSnapshotAndReset();

    IntervalReport report;
    report.timestamp = std::chrono::system_clock::now();
    report.totalConnections = MetricsAggregator::Instance().GetTotalConnections();
    report.snapshot = snap;

    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_history.push_back(report);
    }

    LOG_INFO("[리포트 {}] 누적연결={} RTT(ms) avg={:.3f} p95={:.3f} p99={:.3f} 성공={} 핑타임아웃={} Stuck세션={}",
             FormatTimestampNarrow(report.timestamp, "%H:%M:%S"), report.totalConnections,
             snap.avgMs, snap.p95Ms, snap.p99Ms,
             snap.successCount, snap.timeoutCount, snap.stuckCount);
}

void ReportManager::WriteFinalReport()
{
    // 마지막 10초 미만의 미완결 구간이 그대로 유실되지 않도록, 파일로 쓰기 전에
    // 남은 스냅샷을 한 번 더 히스토리에 반영한다(콘솔에도 마지막 줄로 출력됨).
    ReportInterval();

    std::vector<IntervalReport> history;
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        history = m_history;
    }

    auto now = std::chrono::system_clock::now();
    std::string narrowName = "report_" + FormatTimestampNarrow(now, "%Y%m%d_%H%M%S") + ".csv";
    std::wstring filename(narrowName.begin(), narrowName.end()); // 파일명은 타임스탬프 숫자/영문뿐이라 wide 변환 손실 없음

    std::wofstream ofs(filename);
    if (!ofs.is_open())
    {
        LOG_ERROR("[부하테스트] 리포트 파일 생성 실패: {}", narrowName);
        return;
    }

    ofs << L"timestamp,totalConnections,successCount,timeoutCount,stuckCount,sampleCount,minMs,maxMs,avgMs,p50Ms,p95Ms,p99Ms\n";

    uint64_t totalSuccess = 0, totalTimeout = 0, totalStuck = 0;
    double weightedRttSum = 0.0;
    uint64_t totalSamples = 0;

    for (const auto& r : history)
    {
        const auto& s = r.snapshot;
        ofs << FormatTimestamp(r.timestamp, L"%Y-%m-%d %H:%M:%S") << L","
            << r.totalConnections << L","
            << s.successCount << L"," << s.timeoutCount << L"," << s.stuckCount << L","
            << s.sampleCount << L","
            << s.minMs << L"," << s.maxMs << L"," << s.avgMs << L","
            << s.p50Ms << L"," << s.p95Ms << L"," << s.p99Ms << L"\n";

        totalSuccess += s.successCount;
        totalTimeout += s.timeoutCount;
        totalStuck += s.stuckCount;
        weightedRttSum += s.avgMs * static_cast<double>(s.sampleCount);
        totalSamples += s.sampleCount;
    }

    double overallAvgRtt = totalSamples > 0 ? weightedRttSum / static_cast<double>(totalSamples) : 0.0;

    ofs << L"\n# 요약: 누적연결=" << (history.empty() ? 0 : history.back().totalConnections)
        << L" 총성공=" << totalSuccess
        << L" 총핑타임아웃=" << totalTimeout
        << L" 총Stuck=" << totalStuck
        << L" 전체RTT평균(ms)=" << overallAvgRtt << L"\n";

    ofs.close();

    LOG_INFO("[부하테스트] 최종 리포트 저장 완료: {}", narrowName);
}
