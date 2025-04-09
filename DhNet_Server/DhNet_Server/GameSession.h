#pragma once
#include "../ServerCore/Session.h"
#include "Player.h"

class GameSession : public Session
{
private:
	std::shared_ptr<Player> m_player;

public:
	GameSession();
	~GameSession();

	virtual void OnConnected() override;
	virtual void OnDisconnected() override;
	virtual bool OnRecv(PacketHeader* _packet) override;
	virtual void OnSend(int32 _len) override;

	void SetPlayer(std::shared_ptr<Player> _player) 
	{ 
		m_player = _player; 
		std::cout << "GameSession::SetPlayer() : " << m_player->GetPlayerId() << std::endl;
	}

	std::shared_ptr<Player> GetPlayer() { return m_player; }
};