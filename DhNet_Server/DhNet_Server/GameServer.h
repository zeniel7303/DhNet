#pragma once
#include "../ServerCore/Service.h"
#include "ServerSetting.h"

class GameServer
{
private:
	shared_ptr<ServerService> m_serverService;

private:
	void RegisterPacket();

public:
	GameServer(shared_ptr<ServerSetting> _setting);
	~GameServer();

	void StartServer();
};