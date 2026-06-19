// CRASH 워치독 패치가 실제로 효과 있는지 검증하는 독립 테스트.
// async_logger의 워커 스레드가 영원히 멈춘 상황(디스크/네트워크 I/O 행 시뮬레이션)을
// 커스텀 sink로 재현하고, 큐(용량 8192)를 백그라운드 필러 스레드로 가득 채워서
// "enqueue 자체가 막히는" 실제 시나리오를 만든다. 그 상태에서
// OLD_CRASH(패치 전 코드 그대로)와 NEW_CRASH(현재 Macro.h 코드 그대로)를 비교한다.
//
// 사용법: test_watchdog.exe old   → 패치 전 동작 재현 (영원히 멈춰야 정상 = 버그 재현)
//         test_watchdog.exe new   → 패치 후 동작 재현 (약 500ms 후 크래시해야 정상 = 수정 확인)
//
// 권장 실행 방법:
//   ./test_watchdog.exe new                  → 몇백 ms~1초 안에 크래시(세그폴트)하면 정상
//   timeout 8 ./test_watchdog.exe old         → 8초 동안 살아있으면(타임아웃으로 강제종료, exit 124) 버그 재현 확인

#include <spdlog/spdlog.h>
#include <spdlog/async.h>
#include <spdlog/sinks/base_sink.h>
#include <chrono>
#include <thread>
#include <mutex>
#include <cstdio>
#include <string>

// 워커 스레드가 이 sink에 진입하면 영원히 멈춤 — 디스크/네트워크 I/O 행을 시뮬레이션.
class HangingSink : public spdlog::sinks::base_sink<std::mutex>
{
protected:
    void sink_it_(const spdlog::details::log_msg&) override
    {
        std::this_thread::sleep_for(std::chrono::hours(24));
    }
    void flush_() override
    {
        std::this_thread::sleep_for(std::chrono::hours(24));
    }
};

class Logger
{
public:
    static std::shared_ptr<spdlog::logger>& Get()
    {
        static std::shared_ptr<spdlog::logger> instance = [] {
            spdlog::init_thread_pool(8192, 1);
            auto sink = std::make_shared<HangingSink>();
            return std::make_shared<spdlog::async_logger>(
                "hang_test", spdlog::sinks_init_list{ sink },
                spdlog::thread_pool(), spdlog::async_overflow_policy::block);
        }();
        return instance;
    }
};

void LoggerShutdown()
{
    Logger::Get()->flush();
}

#define LOG_CRITICAL(...) Logger::Get()->critical(__VA_ARGS__)

// 패치 전 실제 코드 그대로 (DhUtil/Macro.h 워치독 패치 이전 버전과 동일 — __analysis_assume만
// 제외했는데, 그건 /analyze 정적분석 전용 무동작 힌트라 런타임 검증과 무관함)
#define OLD_CRASH(cause)                  \
{                                          \
    LOG_CRITICAL("CRASH: {}", cause);      \
    LoggerShutdown();                      \
    unsigned __int32* crash = nullptr;     \
    *crash = 0xDEADBEEF;                   \
}

// 현재 DhUtil/Macro.h의 실제 코드 그대로 (워치독 + try/catch 포함)
#define NEW_CRASH(cause)                                              \
{                                                                      \
    try {                                                              \
        std::thread([] {                                               \
            std::this_thread::sleep_for(std::chrono::milliseconds(500)); \
            unsigned __int32* crash = nullptr;                         \
            *crash = 0xDEADBEEF;                                        \
        }).detach();                                                    \
    } catch (...) {}                                                   \
    LOG_CRITICAL("CRASH: {}", cause);                                   \
    LoggerShutdown();                                                   \
    unsigned __int32* crash = nullptr;                                  \
    *crash = 0xDEADBEEF;                                                \
}

static long long ElapsedMs(std::chrono::steady_clock::time_point start)
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - start).count();
}

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        printf("usage: test_watchdog.exe old|new\n");
        return 1;
    }

    std::string mode = argv[1];

    // 백그라운드 스레드로 계속 로그를 쏟아부어 큐(용량 8192)를 자연스럽게
    // 가득 채운다. 첫 메시지가 워커를 HangingSink 안에 영원히 붙잡아두므로,
    // 워커는 더 이상 큐를 비우지 않고, 이 필러 스레드는 큐가 꽉 차는 순간
    // block 정책에 의해 자기 자신도 enqueue에서 멈춘다 — 그 상태가 되면
    // 큐는 "완전히 가득 찬" 상태로 안정된다.
    std::thread([] {
        int i = 0;
        while (true)
            Logger::Get()->info("filler {}", i++);
    }).detach();

    printf("큐가 가득 찰 시간을 기다리는 중 (300ms)...\n");
    fflush(stdout);
    std::this_thread::sleep_for(std::chrono::milliseconds(300));
    printf("CRASH 진입 — 이 시점에 큐는 가득 차 있어야 함\n");
    fflush(stdout);

    auto start = std::chrono::steady_clock::now();

    printf("[%lldms] CRASH 진입 (mode=%s) - 워커 스레드는 HangingSink에서 영원히 멈춰있고 큐는 가득 참\n",
        ElapsedMs(start), mode.c_str());
    fflush(stdout);

    if (mode == "old")
    {
        OLD_CRASH("TEST");
    }
    else
    {
        NEW_CRASH("TEST");
    }

    // 정상이라면 위에서 크래시했어야 함 — 여기 도달하면 안 됨.
    printf("[%lldms] 비정상: CRASH 이후에도 프로세스가 살아있음!\n", ElapsedMs(start));
    fflush(stdout);
    return 0;
}
