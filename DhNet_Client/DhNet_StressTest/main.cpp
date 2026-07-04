#pragma once
#include "stdafx.h"

#include "../DhNet_Client/ServerSession.h"
#include "../DhNet_Client/LoginController.h"
#include "ScenarioConfig.h"
#include "ScenarioStateRegistry.h"
#include "ScenarioHandlers.h"
#include "MetricsAggregator.h"
#include "ReportManager.h"

#include "../../DhNet_Server/ServerCore/Service.h"
#include "../../DhUtil/ThreadManager.h"

/*
DhNet_StressTest 실행 개요
- 명령줄 인자:
  argv[1] = 서버 IP (기본값: 127.0.0.1)
  argv[2] = 포트 (기본값: 7777)
  argv[3] = 서비스 개수 (기본값: 10)          -> ClientService 풀의 개수
  argv[4] = 서비스당 세션 수 (기본값: 100)     -> 서비스별 동시 세션 대략치(구현에 따라 달라질 수 있음)
  argv[5] = 청소 간격(초) (기본값: 30)          -> 주기마다 약 10%의 서비스를 폐기/재생성하여 유저 입장/퇴장 상황을 모의함
  argv[6] = 로비 채팅 횟수 (기본값: 2)          -> 로비 체류 중 보낼 Req_LobbyChat 횟수
  argv[7] = 로비 채팅 간격(ms) (기본값: 1000)
  argv[8] = 룸 채팅 횟수 (기본값: 5)            -> 룸 입장 후 보낼 Req_RoomChat 횟수
  argv[9] = 룸 채팅 간격(ms) (기본값: 1000)
  argv[10] = 실행 시간(초) (기본값: 0=무제한)    -> 지정하면 경과 시 Ctrl+C와 동일한 경로로 자동 종료(리포트 저장 포함)

설명:
- DhNet_Client의 ServerSession 및 패킷 핸들러를 재사용함.
- 각 세션은 로그인 → 로비 체류(채팅 N회) → 룸 입장 → 룸 채팅 N회 → 룸 퇴장 → (로비로 복귀해 반복)
  시나리오를 독립적으로 수행한다. 진행 상태는 ScenarioStateRegistry가 세션별로 추적한다.
- 서버는 Res_RoomEnter/Res_RoomExit를 실제로 보내지 않으므로(Player::EnterRoom 등 참고),
  룸 입장/퇴장 확인은 각각 Noti_RoomEnter(자기 자신 매칭)와 Res_LobbyEnter 수신으로 판단한다.
- 일부 ClientServiceRef(shared_ptr)를 제거해 소켓이 정리되도록 한 뒤, 동일 개수만큼 다시 생성하여 서비스 풀의 "재생성(churn)"을 주기적으로 시뮬레이션함.
*/

static ThreadManager* GStressThreadMgr = new ThreadManager();

ScenarioConfig g_scenarioConfig;

constexpr int kTickIntervalMs = 200; // 시나리오 진행 체크 주기 (내부 스케줄링 해상도)
constexpr int kReportIntervalSec = 10; // 콘솔 리포트 출력 주기

struct StressConfig
{
    std::wstring ip = L"127.0.0.1";
    uint16_t port = 7777;
    int services = 10;
    int sessionsPerService = 100;
    int churnIntervalSec = 30;
    int runDurationSec = 0; // 0 = 무제한 (Ctrl+C로만 종료)
};

static ClientServiceRef MakeService(const std::wstring& ip, uint16_t port, int sessionsPerService)
{
    return std::make_shared<ClientService>(
        NetAddress(ip.c_str(), port),
        std::make_shared<IocpCore>(),
        []()
        {
            auto session = std::make_shared<ServerSession>();
            ServerSession* raw = session.get();
            session->SetOnConnectedExtra([raw]()
            {
                ScenarioStateRegistry::Instance().Register(raw);
                MetricsAggregator::Instance().RecordConnection();
            });
            session->SetOnDisconnectedExtra([raw]() { ScenarioStateRegistry::Instance().Unregister(raw); });
            return session;
        },
        sessionsPerService);
}

// Ctrl+C / 실행시간 만료 등 어느 경로로 종료하든 항상 이 함수를 거쳐야 리포트/로그가
// 유실되지 않는다. 여러 경로(콘솔 핸들러, 메인 루프의 타임아웃 체크)에서 동시에 호출될
// 수 있으므로 최초 1회만 실제로 종료 절차를 수행한다.
static std::atomic<bool> s_shuttingDown{ false };

static void GracefulShutdown()
{
    if (s_shuttingDown.exchange(true))
        return;

    ReportManager::Instance().WriteFinalReport();

    // LoggerShutdown()(async_logger::flush())은 flush "요청"을 워커 스레드 큐에 넣기만
    // 하고 기다리지 않는 비동기 호출이다(spdlog async_logger-inl.h의 flush_() 참고) —
    // 바로 ExitProcess를 부르면 워커가 그 요청은커녕 직전에 찍은 로그조차 처리하기 전에
    // 프로세스가 죽는 레이스가 실측으로 확인됨("최종 리포트 저장 완료" 로그가 파일에서
    // 누락됨). 워커가 큐를 비울 시간을 잠깐 준 뒤 종료한다.
    LoggerShutdown();
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    ExitProcess(0);
}

// Ctrl+C/Ctrl+Break/콘솔 닫기 시 CSV 리포트를 저장하고 종료한다.
// CTRL_C_EVENT/CTRL_BREAK_EVENT는 핸들러가 TRUE를 반환해도 프로세스가 자동 종료되지
// 않으므로(기본 동작이 억제됨) 반드시 ExitProcess로 직접 끝내야 한다.
static BOOL WINAPI ConsoleHandler(DWORD _signal)
{
    if (_signal == CTRL_C_EVENT || _signal == CTRL_BREAK_EVENT || _signal == CTRL_CLOSE_EVENT)
        GracefulShutdown();
    return TRUE;
}

static void RegisterHandlers()
{
    PacketHandler::Instance().Register(PacketEnum::Res_Login, &HandleResLoginPacket);
    PacketHandler::Instance().Register(PacketEnum::Res_LoginFailed, &HandleResLoginFailedPacket);
    PacketHandler::Instance().Register(PacketEnum::Res_LobbyEnter, &Scenario_HandleResLobbyEnterPacket);
    PacketHandler::Instance().Register(PacketEnum::Noti_RoomEnter, &Scenario_HandleNotiRoomEnterPacket);
    PacketHandler::Instance().Register(PacketEnum::Test, &Scenario_HandleResTestPacket);

    // 로비/룸 멤버들에게 브로드캐스트되는 알림들 — 시나리오 진행에는 필요 없지만,
    // 미등록 상태로 두면 세션이 스스로 "OnRead Error"로 끊어진다.
    PacketHandler::Instance().Register(PacketEnum::Noti_LobbyChat, &Scenario_HandleIgnoredBroadcastPacket);
    PacketHandler::Instance().Register(PacketEnum::Noti_LobbyPlayerEnter, &Scenario_HandleIgnoredBroadcastPacket);
    PacketHandler::Instance().Register(PacketEnum::Noti_LobbyPlayerExit, &Scenario_HandleIgnoredBroadcastPacket);
    PacketHandler::Instance().Register(PacketEnum::Noti_RoomChat, &Scenario_HandleIgnoredBroadcastPacket);
    PacketHandler::Instance().Register(PacketEnum::Noti_RoomExit, &Scenario_HandleIgnoredBroadcastPacket);
}

int wmain(int argc, wchar_t* argv[])
{
    Logger::SetLogFileName("dhnet-stresstest.log"); // DhNet_Server/DhNet_Client와 로그 파일을 공유하지 않도록 분리

    StressConfig cfg;
    if (argc > 1) cfg.ip = argv[1];
    if (argc > 2) cfg.port = static_cast<uint16_t>(_wtoi(argv[2]));
    if (argc > 3) cfg.services = _wtoi(argv[3]);
    if (argc > 4) cfg.sessionsPerService = _wtoi(argv[4]);
    if (argc > 5) cfg.churnIntervalSec = _wtoi(argv[5]);
    if (argc > 6) g_scenarioConfig.lobbyChatCount = _wtoi(argv[6]);
    if (argc > 7) g_scenarioConfig.lobbyChatIntervalMs = _wtoi(argv[7]);
    if (argc > 8) g_scenarioConfig.roomChatCount = _wtoi(argv[8]);
    if (argc > 9) g_scenarioConfig.roomChatIntervalMs = _wtoi(argv[9]);
    if (argc > 10) cfg.runDurationSec = _wtoi(argv[10]);

    SetConsoleCtrlHandler(ConsoleHandler, TRUE);

    // 서버가 준비될 시간을 잠시 대기
    std::this_thread::sleep_for(std::chrono::seconds(2));

    Sender::Init(4096 * 8);
    RegisterHandlers();

    // 서비스 풀 생성
    std::vector<ClientServiceRef> services;
    services.reserve(cfg.services);

    for (int i = 0; i < cfg.services; ++i)
    {
        auto svc = MakeService(cfg.ip, cfg.port, cfg.sessionsPerService);
        ASSERT_CRASH(svc->Start());
        services.push_back(svc);
    }

    // IOCP 디스패치 스레드 실행
    const int dispatchThreads = max(4, (int)std::thread::hardware_concurrency());
    for (int i = 0; i < dispatchThreads; ++i)
    {
        GStressThreadMgr->Launch([&services]()
        {
            // 각 스레드는 서비스들을 순환하며 Dispatch 호출
            size_t idx = 0;
            while (true)
            {
                if (services.empty())
                {
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                    continue;
                }
                auto& svc = services[idx % services.size()];
                svc->GetIocpCore()->Dispatch();
                idx++;
            }
        });
    }

    // 시나리오 진행과 재생성(churn)을 위한 타이머
    auto lastChurn = std::chrono::steady_clock::now();
    auto lastReport = std::chrono::steady_clock::now();
    auto startTime = std::chrono::steady_clock::now();

    uint64_t tick = 0;
    while (true)
    {
        // 모든 세션의 시나리오 상태를 확인해 다음 단계로 진행시킴
        auto now = std::chrono::steady_clock::now();
        for (auto& svc : services)
        {
            for (auto& session : svc->GetSessions())
            {
                AdvanceScenario(session, now);
                AdvancePing(session, now);
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(kTickIntervalMs));

        // 주기적 재생성: 서비스의 약 10%를 폐기 후 다시 생성
        auto nowChurn = std::chrono::steady_clock::now();
        if (std::chrono::duration_cast<std::chrono::seconds>(nowChurn - lastChurn).count() >= cfg.churnIntervalSec)
        {
            lastChurn = nowChurn;

            int toRecycle = max(1, (int)(services.size() / 10));
            for (int i = 0; i < toRecycle && !services.empty(); ++i)
            {
                // 하나의 서비스를 종료 후 제거하여 소켓/세션을 명시적으로 정리
                auto svc = services.front();
                if (svc)
                    svc->End();
                services.erase(services.begin());
            }
            // 동일한 개수만큼 다시 생성
            for (int i = 0; i < toRecycle; ++i)
            {
                auto svc = MakeService(cfg.ip, cfg.port, cfg.sessionsPerService);
                ASSERT_CRASH(svc->Start());
                services.push_back(svc);
            }

            // 상태 확인 로그
            LOG_INFO("[부하테스트] 서비스 재생성 완료. 서비스 수: {}, 틱: {}", services.size(), tick);
        }

        // 주기적 콘솔 리포트 (10초 간격)
        auto nowReport = std::chrono::steady_clock::now();
        if (std::chrono::duration_cast<std::chrono::seconds>(nowReport - lastReport).count() >= kReportIntervalSec)
        {
            lastReport = nowReport;
            ReportManager::Instance().ReportInterval();
        }

        // 지정된 실행 시간이 지나면 Ctrl+C와 동일한 경로(리포트 저장 + 로그 flush)로 자동 종료
        if (cfg.runDurationSec > 0 &&
            std::chrono::duration_cast<std::chrono::seconds>(nowReport - startTime).count() >= cfg.runDurationSec)
        {
            LOG_INFO("[부하테스트] 지정된 실행 시간({}초) 경과 — 자동 종료", cfg.runDurationSec);
            GracefulShutdown();
        }

        tick++;
    }

    GStressThreadMgr->Join();

    return 0;
}
