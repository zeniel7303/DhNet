#pragma once
#include "stdafx.h"

#include "../DhNet_Client/ServerSession.h"
#include "../DhNet_Client/TestController.h"
#include "../DhNet_Client/LoginController.h"
#include "../DhNet_Client/RoomController.h"

#include "../../DhNet_Server/ServerCore/Service.h"
#include "../../DhUtil/ThreadManager.h"

#define STRESS_TEST

/*
DhNet_StressTest 실행 개요
- 명령줄 인자:
  argv[1] = 서버 IP (기본값: 127.0.0.1)
  argv[2] = 포트 (기본값: 7777)
  argv[3] = 서비스 개수 (기본값: 10)          -> ClientService 풀의 개수
  argv[4] = 서비스당 세션 수 (기본값: 100)     -> 서비스별 동시 세션 대략치(구현에 따라 달라질 수 있음)
  argv[5] = 초당 메시지 수 (기본값: 2)         -> 서비스당 브로드캐스트 주기
  argv[6] = 청소 간격(초) (기본값: 30)          -> 주기마다 약 10%의 서비스를 폐기/재생성하여 유저 입장/퇴장 상황을 모의함

설명:
- DhNet_Client의 ServerSession 및 패킷 핸들러를 재사용함.
- 일부 ClientServiceRef(shared_ptr)를 제거해 소켓이 정리되도록 한 뒤, 동일 개수만큼 다시 생성하여 서비스 풀의 "재생성(치른)"을 주기적으로 시뮬레이션함.
*/

static ThreadManager* GStressThreadMgr = new ThreadManager();

struct StressConfig
{
    std::wstring ip = L"127.0.0.1";
    uint16_t port = 7777;
    int services = 10;
    int sessionsPerService = 100;
    int msgPerSecond = 2;
    int churnIntervalSec = 30;
};

static ClientServiceRef MakeService(const std::wstring& ip, uint16_t port, int sessionsPerService)
{
    return std::make_shared<ClientService>(
        NetAddress(ip.c_str(), port),
        std::make_shared<IocpCore>(),
        []() { return std::make_shared<ServerSession>(); },
        sessionsPerService);
}

static void RegisterHandlers()
{
    PacketHandler::Instance().Register(PacketEnum::Test, &RecvTestPacket);
    PacketHandler::Instance().Register(PacketEnum::Res_Login, &HandleResLoginPacket);
    PacketHandler::Instance().Register(PacketEnum::Noti_RoomEnter, &HandleNotiRoomEnterPacket);
    PacketHandler::Instance().Register(PacketEnum::Noti_RoomChat, &HandleNotiRoomChatPacket);
    PacketHandler::Instance().Register(PacketEnum::Noti_RoomExit, &HandleNotiRoomExitPacket);
}

int wmain(int argc, wchar_t* argv[])
{
    StressConfig cfg;
    if (argc > 1) cfg.ip = argv[1];
    if (argc > 2) cfg.port = static_cast<uint16_t>(_wtoi(argv[2]));
    if (argc > 3) cfg.services = _wtoi(argv[3]);
    if (argc > 4) cfg.sessionsPerService = _wtoi(argv[4]);
    if (argc > 5) cfg.msgPerSecond = _wtoi(argv[5]);
    if (argc > 6) cfg.churnIntervalSec = _wtoi(argv[6]);

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

    // 테스트 패킷을 미리 생성(템플릿)
    auto senderAndPacket = Sender::GetSenderAndPacket<TestPacket>();
    senderAndPacket.first->Init(PacketEnum::Test, sizeof(TestPacket));
    strcpy_s(senderAndPacket.first->m_test, "StressTest(Client)");

    // 브로드캐스트와 재생성(치른)을 위한 타이머
    auto lastChurn = std::chrono::steady_clock::now();
    const int sleepMs = max(1, 1000 / max(1, cfg.msgPerSecond));

    uint64_t tick = 0;
    while (true)
    {
        // 설정된 빈도로 서비스마다 브로드캐스트 전송
        for (auto& svc : services)
            svc->BroadCast(senderAndPacket.second);

        std::this_thread::sleep_for(std::chrono::milliseconds(sleepMs));

        // 주기적 재생성: 서비스의 약 10%를 폐기 후 다시 생성
        auto now = std::chrono::steady_clock::now();
        if (std::chrono::duration_cast<std::chrono::seconds>(now - lastChurn).count() >= cfg.churnIntervalSec)
        {
            lastChurn = now;

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
            std::wcout << L"[부하테스트] 서비스 재생성 완료. 서비스 수: " << services.size() << L", 틱: " << tick << std::endl;
        }

        tick++;
    }

    GStressThreadMgr->Join();

    return 0;
}
