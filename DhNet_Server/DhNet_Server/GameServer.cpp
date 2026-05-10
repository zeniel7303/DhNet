#include "stdafx.h"
#include "../../DhUtil/ThreadManager.h"
#include "Controllers.h"

#include "GameServer.h"
#include "GameSession.h"

#include "Room.h"
#include "AdminGrpcServer.h"

GameServer GameServer::m_singleton;

GameServer::GameServer()
{
	m_maxSessionCount = 0;
	m_port = 0;
}

GameServer::~GameServer()
{
    if (m_dbSystem)
        m_dbSystem->Shutdown();
}

GameServer& GameServer::Instance()
{
    return m_singleton;
}

void GameServer::AddSetting(std::shared_ptr<ServerSetting> _setting)
{
    m_setting = _setting;
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
    PacketHandler::Instance().Register(PacketEnum::Req_LobbyChat, &HandleReqLobbyChatPacket);
    PacketHandler::Instance().Register(PacketEnum::Req_RoomList, &HandleReqRoomListPacket);
}

void GameServer::AddSystem()
{
	m_gameSessionSystem = std::make_unique<GameSessionSystem>();
	m_playerSystem = std::make_unique<PlayerSystem>();
	m_roomSystem = std::make_unique<RoomSystem>();
	m_lobbySystem = std::make_unique<LobbySystem>();

	m_dbSystem = std::make_unique<DbSystem>();
	if (m_setting)
		m_dbSystem->Init(*m_setting);
}

void GameServer::StartServer()
{
    RegisterPacket();

    AddSystem();

    m_serverService = std::make_shared<ServerService>(
        NetAddress(m_ip, m_port),
        std::make_shared<IocpCore>(),
        []() { return ObjectPool<GameSession>::MakeShared(); },
        // []() { return std::make_shared<GameSession>(); },
        m_maxSessionCount);

    ASSERT_CRASH(m_serverService->Start());

    GrpcHost::Instance().Start(m_setting->GetGrpcAddress());

    int32 workerCount = static_cast<int32>(std::thread::hardware_concurrency());
    for (int32 i = 0; i < workerCount; i++)
    {
        GThreadManager->Launch([=]()
            {
                Job();
            });
    }

    GThreadManager->Join();
}

void GameServer::Job()
{
    while (true)
    {
        LEndTickCount = ::GetTickCount64() + 64;

        m_serverService->GetIocpCore()->Dispatch(10);

        ThreadManager::DoGlobalQueueWork();
    }
}