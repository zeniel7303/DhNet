#pragma once
#include "../../DhNet_Server/ServerCore/Session.h"
#include "../../DhNet_Server/ServerCore/PacketHandler.h"

#include "../../DhNet_Server/DhNet_Protocol/PacketList.h"
#include "../../DhNet_Server/DhNet_Protocol/PacketEnum.h"

class ServerSession : public Session
{
public:
	uint64 m_playerId;
	string m_playerName;

public:
	~ServerSession()
	{
		cout << "~ServerSession()" << endl;
	}

	virtual void OnConnected() override;
	virtual bool OnRecv(PacketHeader* _packet) override;
	virtual void OnSend(int32 _len) override;
	virtual void OnDisconnected() override;
};