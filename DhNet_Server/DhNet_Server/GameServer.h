#pragma once
#include "../ServerCore/Service.h"
#include "ServerSetting.h"

#include "UniqueIdGenerationSystem.h"
#include "GameSessionSystem.h"
#include "PlayerSystem.h"
#include "RoomSystem.h"
#include "LobbySystem.h"

class GameServer
{
private:
	static GameServer m_singleton;

	std::wstring m_ip;
	uint16 m_port;
	int32 m_maxSessionCount;

	ServerServiceRef m_serverService;

	std::unique_ptr<UniqueIdGenerationSystem> m_uniqueIdGenerationSystem;
	std::unique_ptr<GameSessionSystem> m_gameSessionSystem;
	std::unique_ptr<PlayerSystem> m_playerSystem;
	std::unique_ptr<RoomSystem> m_roomSystem;
	std::unique_ptr<LobbySystem> m_lobbySystem;

private:
	void RegisterPacket();
	void AddSystem();

public:
	GameServer();
	~GameServer();

	static GameServer& Instance();

	void AddSetting(std::shared_ptr<ServerSetting> _setting);
	void StartServer();
	void Job();

	template <typename T>
	T* GetSystem();
};

#include "GameServer.hpp"