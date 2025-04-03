#pragma once
#include "../ServerCore/Service.h"
#include "ServerSetting.h"

class GameServer
{
private:
	ServerServiceRef m_serverService;

private:
	void RegisterPacket();

public:
	GameServer(std::shared_ptr<ServerSetting> _setting);
	~GameServer();

	void StartServer();
};