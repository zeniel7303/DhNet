#include "stdafx.h"
#include "GameServer.h"
#include "GameSession.h"
#include "ServerPacketHandler.h"
#include "../../DhUtil/ThreadManager.h"

GameServer::GameServer(shared_ptr<ServerSetting> _setting)
{
    RegisterPacket();

	auto ip = _setting->GetIp();
	auto port = _setting->GetPort();
	auto maxSessionCount = _setting->GetMaxSessionCount();

    serverService = make_shared<ServerService>(
        NetAddress(ip, port),
        make_shared<IocpCore>(),
        []() { return make_shared<GameSession>(); },
        maxSessionCount);
}

GameServer::~GameServer()
{
	
}

void GameServer::RegisterPacket()
{
    PacketHandler::Instance().Register(PacketEnum::Test, &HandleTestPacket);
}

void GameServer::StartServer()
{
    ASSERT_CRASH(serverService->Start());

    for (int32 i = 0; i < 5; i++)
    {
        GThreadManager->Launch([=]()
            {
                while (true)
                {
                    // 자동으로 Listener의 Dispatch도 들어간다.(가상함수이므로)
                    serverService->GetIocpCore()->Dispatch();
                }
            });
    }

    GThreadManager->Join();
}