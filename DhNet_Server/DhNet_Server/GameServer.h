#pragma once
#include "../ServerCore/Service.h"
#include "ServerSetting.h"

#include "GameSessionSystem.h"
#include "PlayerSystem.h"
#include "RoomSystem.h"
#include "LobbySystem.h"
#include "DbSystem.h"

class GameServer
{
private:
	static GameServer m_singleton;

	std::wstring m_ip;
	uint16 m_port;
	int32 m_maxSessionCount;

	std::shared_ptr<ServerSetting> m_setting;

	ServerServiceRef m_serverService;

	std::unique_ptr<GameSessionSystem> m_gameSessionSystem;
	std::unique_ptr<PlayerSystem> m_playerSystem;
	std::unique_ptr<RoomSystem> m_roomSystem;
	std::unique_ptr<LobbySystem> m_lobbySystem;
	std::unique_ptr<DbSystem> m_dbSystem;

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