#include "stdafx.h"
#include "../../DhUtil/ThreadManager.h"
#include "Controllers.h"

#include "GameServer.h"
#include "GameSession.h"

#include "Room.h"

GameServer GameServer::m_singleton;

GameServer::GameServer()
{

}

GameServer::~GameServer()
{
	
}

GameServer& GameServer::Instance()
{
    return m_singleton;
}

void GameServer::AddSetting(std::shared_ptr<ServerSetting> _setting)
{
    m_ip = _setting->GetIp();
    m_port = _setting->GetPort();
    m_maxSessionCount = _setting->GetMaxSessionCount();
}

void GameServer::RegisterPacket()
{
    PacketHandler::Instance().Register(PacketEnum::Test, &HandleTestPacket);
    PacketHandler::Instance().Register(PacketEnum::Req_RoomEnter, &HandleReqRoomEnterPacket);
    PacketHandler::Instance().Register(PacketEnum::Req_Login, &HandleReqLoginPacket);
    PacketHandler::Instance().Register(PacketEnum::Req_RoomChat, &HandleReqRoomChatPacket);
    PacketHandler::Instance().Register(PacketEnum::Req_RoomExit, &HandleReqRoomExitPacket);
}

void GameServer::AddSystem()
{
	m_uniqueIdGenerationSystem = new UniqueIdGenerationSystem();
	m_gameSessionSystem = new GameSessionSystem();
	m_playerSystem = new PlayerSystem();
}

void GameServer::StartServer()
{
    RegisterPacket();

    AddSystem();

    m_serverService = std::make_shared<ServerService>(
        NetAddress(m_ip, m_port),
        std::make_shared<IocpCore>(),
        []() { return std::make_shared<GameSession>(); },
        m_maxSessionCount);

    ASSERT_CRASH(m_serverService->Start());

    for (int32 i = 0; i < 5; i++)
    {
        GThreadManager->Launch([=]()
            {
                while (true)
                {
                    // �ڵ����� Listener�� Dispatch�� ����.(�����Լ��̹Ƿ�)
                    m_serverService->GetIocpCore()->Dispatch();
                }
            });
    }

    while (true)
    {
        GRoom.FlushJob();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    GThreadManager->Join();
}