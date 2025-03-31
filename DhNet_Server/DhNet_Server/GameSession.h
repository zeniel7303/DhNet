#pragma once
#include "../ServerCore/Session.h"
#include "Player.h"

class GameSession : public Session
{
private:
	shared_ptr<Player> m_player;

public:
	~GameSession()
	{
		cout << "~GameSession" << endl;
	}

	virtual void OnConnected() override;
	virtual void OnDisconnected() override;
	virtual bool OnRecv(PacketHeader* _packet) override;
	virtual void OnSend(int32 _len) override;

	void SetPlayer(shared_ptr<Player> _player) { m_player = _player; }
	shared_ptr<Player> GetPlayer() { return m_player; }
};