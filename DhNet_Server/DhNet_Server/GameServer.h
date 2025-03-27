#pragma once
#include "../ServerCore/Service.h"
#include "ServerSetting.h"

class GameServer
{
private:
	shared_ptr<ServerService> serverService;

private:
	void RegisterPacket();

public:
	GameServer(shared_ptr<ServerSetting> _setting);
	~GameServer();

	void StartServer();
};