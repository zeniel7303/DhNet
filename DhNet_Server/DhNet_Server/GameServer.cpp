#include "stdafx.h"
#include "../../DhUtil/ThreadManager.h"

#include "GameServer.h"
#include "GameSession.h"

#include "TestController.h"
#include "RoomController.h"
#include "LoginController.h"

GameServer::GameServer(shared_ptr<ServerSetting> _setting)
{
    RegisterPacket();

	auto ip = _setting->GetIp();
	auto port = _setting->GetPort();
	auto maxSessionCount = _setting->GetMaxSessionCount();

    m_serverService = make_shared<ServerService>(
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
    PacketHandler::Instance().Register(PacketEnum::Req_RoomEnter, &HandleReqRoomEnterPacket);
    PacketHandler::Instance().Register(PacketEnum::Req_Login, &HandleReqLoginPacket);
    PacketHandler::Instance().Register(PacketEnum::Req_RoomChat, &HandleReqRoomChatPacket);
    PacketHandler::Instance().Register(PacketEnum::Req_RoomExit, &HandleReqRoomExitPacket);
}

void GameServer::StartServer()
{
    ASSERT_CRASH(m_serverService->Start());

    for (int32 i = 0; i < 5; i++)
    {
        GThreadManager->Launch([=]()
            {
                while (true)
                {
                    // 자동으로 Listener의 Dispatch도 들어간다.(가상함수이므로)
                    m_serverService->GetIocpCore()->Dispatch();
                }
            });
    }

    GThreadManager->Join();
}